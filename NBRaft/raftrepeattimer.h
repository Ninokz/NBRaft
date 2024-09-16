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
				: m_interval(boost::asio::chrono::seconds(interval_seconds)),
				m_callback(std::forward<F>(func)),
				m_ioc(),
				m_work_guard(boost::asio::make_work_guard(m_ioc)),
				m_timer(m_ioc),
				m_running(false) {}

			RepeatingTimer(RepeatingTimer&& other) noexcept
				: m_interval(other.m_interval),
				m_callback(std::move(other.m_callback)),
				m_ioc(),
				m_work_guard(boost::asio::make_work_guard(m_ioc)),
				m_timer(m_ioc),
				m_running(other.m_running.load()) {
				if (m_running) {
					start();
				}
			}

			RepeatingTimer& operator=(RepeatingTimer&& other) noexcept {
				if (this != &other) {
					stop();
					m_interval = other.m_interval;
					m_callback = std::move(other.m_callback);
					m_running = other.m_running.load();
					if (m_running) {
						start();
					}
				}
				return *this;
			}

			void start() {
				if (m_running.exchange(true)) {
					return;
				}

				m_timer_thread = std::thread([this]() {
					m_ioc.run();
					});

				m_timer.expires_after(m_interval);
				m_timer.async_wait([this](const boost::system::error_code& ec) {
					this->handle_timer(ec);
					});
			}

			void stop() {
				if (!m_running.exchange(false)) {
					return;
				}

				m_ioc.stop();
				if (m_timer_thread.joinable()) {
					m_timer_thread.join();
				}
			}

			~RepeatingTimer() {
				stop();
			}

		private:
			void handle_timer(const boost::system::error_code& ec) {
				if (!ec && m_running) {
					m_callback();
					m_timer.expires_after(m_interval);
					m_timer.async_wait([this](const boost::system::error_code& ec) {
						this->handle_timer(ec);
						});
				}
			}

			boost::asio::chrono::seconds m_interval;
			Func m_callback;
			boost::asio::io_context m_ioc;
			boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_work_guard;
			boost::asio::steady_timer m_timer;
			std::thread m_timer_thread;
			std::atomic<bool> m_running;
		};

		template<typename Func>
		auto make_repeating_timer(int interval_seconds, Func&& func) {
			return RepeatingTimer<std::decay_t<Func>>(interval_seconds, std::forward<Func>(func));
		}
	}
}