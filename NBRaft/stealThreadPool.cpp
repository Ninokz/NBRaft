#include "stealThreadPool.h"

namespace Nano {
	namespace Concurrency {
		StealThreadPool::StealThreadPool(int maxFailCount, int maxBackOffTime, unsigned int poolSize) :
			m_done(false), m_joiner(m_threads), m_atm_index(0),
			m_maxFailCount(maxFailCount),
			m_maxBackOffTimeMilliSeconds(maxBackOffTime)
		{
			try
			{
				m_thread_work_ques = std::vector <Concurrency::ConcurrentQueue<FunctionWrapper>>(poolSize);

				for (unsigned i = 0; i < poolSize; ++i)
				{
					m_threads.push_back(std::thread(&StealThreadPool::worker_thread, this, i));
				}
			}
			catch (...)
			{
				m_done = true;
				for (int i = 0; i < m_thread_work_ques.size(); i++) {
					m_thread_work_ques[i].Exit();
				}
				throw;
			}
		}

		void StealThreadPool::worker_thread(int index)
		{
			int fail_count = 0;
			int backoff_time_ms = 1;
			while (!m_done)
			{
				FunctionWrapper wrapper;
				if (m_thread_work_ques[index].try_pop(wrapper) || try_steal_from_others(index, wrapper)) {
					// �ɹ���ȡ����������˱�ʱ��, ����ʧ�ܴ���
					fail_count = 0;
					backoff_time_ms = 1;
					wrapper();
				}
				else {
					// δ��ȡ������ʧ�ܴ�����1
					++fail_count;
					if (fail_count > m_maxFailCount) {
						// ����һ��ʱ��
						std::this_thread::sleep_for(std::chrono::milliseconds(backoff_time_ms));
						// �˱�ʱ�䷭��, ָ���˱ܲ��ԣ���ൽ m_maxBackOffTimeMilliSeconds ����
						backoff_time_ms = std::min(backoff_time_ms * 2, m_maxBackOffTimeMilliSeconds);
						// fail_count = 0;	// ����ʧ�ܴ���
						// ������ fail_count���������Գ������� backoff_time_ms��ֱ���ﵽ����
						// ���������������̵߳�����ʱ�䣬���̹߳�������ʧ�ܴ����ᱻ����
					}
					else {
						// δ��ȡ�������߳��ó�CPU
						std::this_thread::yield();
					}
				}
			}
		}

		StealThreadPool::~StealThreadPool()
		{
			m_done = true;
			for (unsigned i = 0; i < m_thread_work_ques.size(); i++) {
				m_thread_work_ques[i].Exit();
			}

			for (unsigned i = 0; i < m_threads.size(); ++i)
			{
				m_threads[i].join();
			}
		}

		bool StealThreadPool::try_steal_from_others(int selfIndex, FunctionWrapper& wrapper)
		{
			for (int i = 0; i < m_thread_work_ques.size(); ++i) {
				if (i != selfIndex && m_thread_work_ques[i].try_steal(wrapper)) {
					return true;
				}
			}
			return false;
		}
	}
}