#include <boost/asio.hpp>
#include <thread>
#include <functional>
#include <iostream>
#include <atomic>

namespace Nano {
    namespace Raft {
        class RepeatingTimer {
        public:
            RepeatingTimer(int interval_seconds, std::function<void()> callback);
            void start();
            void stop();
            ~RepeatingTimer();
        private:
            void handle_timer(const boost::system::error_code& ec);                                         // 定时器的处理函数   
            boost::asio::io_context io_context_;                                                            // io_context 对象，用于管理异步事件
            boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;           // work_guard 保持 io_context 的工作状态，使其不会自动停止    
            boost::asio::steady_timer timer_;               // 定时器对象     
            boost::asio::chrono::seconds interval_;         // 定时器时间间隔       
            std::function<void()> callback_;                // 用户传入的回调函数
            std::thread timer_thread_;                      // 定时器线程
            std::atomic<bool> running_;                     // 标志定时器是否在运行
        };
    }
}