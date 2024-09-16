#pragma once
#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <functional>

#include "nocopyable.h"
#include "singleton.h"

namespace Nano {
	namespace Concurrency {
		class SimpleThreadPool : public Singleton<SimpleThreadPool>, public Noncopyable
		{
			friend class Singleton<SimpleThreadPool>;
			using Task = std::packaged_task<void()>;
		private:
			std::atomic_int m_threadNum;
			std::queue<Task> m_tasks;
			std::vector<std::thread> m_pool;
			std::atomic_bool m_stop;

			std::mutex  m_cv_mt;
			std::condition_variable m_cv_lock;
		private:
			SimpleThreadPool(int poolSize = std::thread::hardware_concurrency());
			void start();
			void stop();
		public:
			virtual ~SimpleThreadPool();

			template<class F, class... Args>
			auto submit(F&& f, Args&&... args) ->
				std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))> {
				using ReturnType = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
				if (m_stop.load())
					return std::future<ReturnType>();

				auto task = std::make_shared<std::packaged_task<ReturnType()>>(
					std::bind(std::forward<F>(f), std::forward<Args>(args)...)
				);

				std::future<ReturnType> res = task->get_future();
				{
					std::lock_guard<std::mutex> lock(m_cv_mt);
					m_tasks.emplace([task]() { (*task)(); });
				}
				m_cv_lock.notify_one();
				return res;
			}

			int getIdleThreadCount() const { return m_threadNum.load(); }
		};
	}
}