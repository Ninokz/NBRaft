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

            // 启动线程来处理 io_context
            timer_thread_ = std::thread([this]() {
                io_context_.run();
            });

            // 设置定时器的初始时间
            timer_.expires_after(interval_);
            timer_.async_wait([this](const boost::system::error_code& ec) {
                this->handle_timer(ec);
                });
        }

        void RepeatingTimer::stop() {
            if (!running_.exchange(false)) {
                return; // 已经停止，无需再次停止
            }

            // 停止 io_context，并等待线程结束
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