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
            void handle_timer(const boost::system::error_code& ec);                                         // ��ʱ���Ĵ�����   
            boost::asio::io_context io_context_;                                                            // io_context �������ڹ����첽�¼�
            boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;           // work_guard ���� io_context �Ĺ���״̬��ʹ�䲻���Զ�ֹͣ    
            boost::asio::steady_timer timer_;               // ��ʱ������     
            boost::asio::chrono::seconds interval_;         // ��ʱ��ʱ����       
            std::function<void()> callback_;                // �û�����Ļص�����
            std::thread timer_thread_;                      // ��ʱ���߳�
            std::atomic<bool> running_;                     // ��־��ʱ���Ƿ�������
        };
    }
}