#include "parrallenThreadPool.h"

namespace Nano {
	namespace Concurrency {
		ParrallenThreadPool::ParrallenThreadPool(unsigned int poolSize) :
			m_done(false),
			m_joiner(m_threads),
			m_atm_index(0)
		{
			try
			{
				m_thread_work_ques = std::vector<Concurrency::ConcurrentQueue<FunctionWrapper>>(poolSize);
				for (unsigned int i = 0; i < poolSize; ++i)
				{
					m_threads.push_back(std::thread(&ParrallenThreadPool::worker_thread, this, i));
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

		ParrallenThreadPool::~ParrallenThreadPool()
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

		void ParrallenThreadPool::worker_thread(int index)
		{
			while (!m_done)
			{
				auto task_ptr = m_thread_work_ques[index].wait_and_pop();
				if (task_ptr == nullptr) {
					continue;
				}
				(*task_ptr)();
			}
		}
	}
}