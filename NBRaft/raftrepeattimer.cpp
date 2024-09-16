#include "raftrepeattimer.h"


namespace Nano {
    namespace Raft {
        RepeatingTimer::RepeatingTimer(int interval_seconds, std::function<void()> callback)
            : interval_(boost::asio::chrono::seconds(interval_seconds)),
            callback_(callback),
            work_guard_(boost::asio::make_work_guard(io_context_)),
            timer_(io_context_),
            running_(false) {}

        void RepeatingTimer::start() {
            if (running_.exchange(true)) {
                return;
            }

            // �����߳������� io_context
            timer_thread_ = std::thread([this]() {
                io_context_.run();
            });

            // ���ö�ʱ���ĳ�ʼʱ��
            timer_.expires_after(interval_);
            timer_.async_wait([this](const boost::system::error_code& ec) {
                this->handle_timer(ec);
                });
        }

        void RepeatingTimer::stop() {
            if (!running_.exchange(false)) {
                return; // �Ѿ�ֹͣ�������ٴ�ֹͣ
            }

            // ֹͣ io_context�����ȴ��߳̽���
            io_context_.stop();
            if (timer_thread_.joinable()) {
                timer_thread_.join();
            }
        }

        RepeatingTimer::~RepeatingTimer() {
            stop();
        }

        void RepeatingTimer::handle_timer(const boost::system::error_code& ec) {
            if (!ec && running_) {
                callback_();
                timer_.expires_after(interval_);
                timer_.async_wait([this](const boost::system::error_code& ec) {
                    this->handle_timer(ec);
                    });
            }
        }
    }
}