#include <thread>
#include <atomic>
#include <functional>
#include <type_traits>

#include <boost/asio.hpp>

#include "nocopyable.h"

namespace Nano {
	namespace Raft {
        template<typename Func>
		class RepeatingTimer : public Nano::Noncopyable
        {
        public:
            template<typename F>
            RepeatingTimer(int interval_seconds, F&& func)
                : interval_(boost::asio::chrono::seconds(interval_seconds)),
                callback_(std::forward<F>(func)),
                io_context_(),
                work_guard_(boost::asio::make_work_guard(io_context_)),
                timer_(io_context_),
                running_(false) {}

            RepeatingTimer(RepeatingTimer&& other) noexcept
                : interval_(other.interval_),
                callback_(std::move(other.callback_)),
                io_context_(),
                work_guard_(boost::asio::make_work_guard(io_context_)),
                timer_(io_context_),
                running_(other.running_.load()) {
                if (running_) {
                    start();
                }
            }

            RepeatingTimer& operator=(RepeatingTimer&& other) noexcept {
                if (this != &other) {
                    stop();
                    interval_ = other.interval_;
                    callback_ = std::move(other.callback_);
                    running_ = other.running_.load();
                    if (running_) {
                        start();
                    }
                }
                return *this;
            }

            void start() {
                if (running_.exchange(true)) {
                    return;
                }

                timer_thread_ = std::thread([this]() {
                    io_context_.run();
                    });

                timer_.expires_after(interval_);
                timer_.async_wait([this](const boost::system::error_code& ec) {
                    this->handle_timer(ec);
                    });
            }

            void stop() {
                if (!running_.exchange(false)) {
                    return;
                }

                io_context_.stop();
                if (timer_thread_.joinable()) {
                    timer_thread_.join();
                }
            }

            ~RepeatingTimer() {
                stop();
            }

        private:
            void handle_timer(const boost::system::error_code& ec) {
                if (!ec && running_) {
                    callback_();
                    timer_.expires_after(interval_);
                    timer_.async_wait([this](const boost::system::error_code& ec) {
                        this->handle_timer(ec);
                        });
                }
            }

            boost::asio::chrono::seconds interval_;
            Func callback_;
            boost::asio::io_context io_context_;
            boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;
            boost::asio::steady_timer timer_;
            std::thread timer_thread_;
            std::atomic<bool> running_;
        };

        // Helper function to create a RepeatingTimer
        template<typename Func>
        auto make_repeating_timer(int interval_seconds, Func&& func) {
            return RepeatingTimer<std::decay_t<Func>>(interval_seconds, std::forward<Func>(func));
        }
	}
}