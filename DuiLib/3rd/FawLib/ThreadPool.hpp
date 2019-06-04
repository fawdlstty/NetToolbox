////////////////////////////////////////////////////////////////////////////////
//
// Class Name:  ThreadPool
// Description: 线程池类
// Class URI:   https://github.com/fawdlstty/FawLib
// Author:      Fawdlstty
// Author URI:  https://www.fawdlstty.com/
// License:     MIT
// Last Update: Apr 02, 2019
//
////////////////////////////////////////////////////////////////////////////////

#ifndef FAWLIB__THREAD_POOL_HPP__
#define FAWLIB__THREAD_POOL_HPP__



#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>


namespace faw {
	class ThreadPool {
	public:
		ThreadPool (size_t _threads) {
			for (size_t i = 0; i < _threads; ++i) {
				m_workers.emplace_back ([this] {
					while (true) {
						std::unique_lock<std::mutex> ul (m_mutex);
						m_condition.wait (ul, [this] { return !m_run || !m_tasks.empty (); });
						if (!m_run)
							return;
						std::function<void ()> _task = std::move (m_tasks.front ());
						m_tasks.pop ();
						ul.unlock ();
						_task ();
					}
				});
			}
		}

		~ThreadPool () {
			m_run = false;
			m_condition.notify_all ();
			for (std::thread &_worker : m_workers)
				_worker.join ();
		}

		template<class F, class... Args>
		auto enqueue (F&& f, Args&&... args) ->std::future<typename std::result_of<F (Args...)>::type> {
			using return_type = typename std::result_of<F (Args...)>::type;
			auto _task = std::make_shared<std::packaged_task<return_type ()>> (std::bind (std::forward<F> (f), std::forward<Args> (args)...));
			std::future<return_type> _res = _task->get_future ();
			std::unique_lock<std::mutex> ul (m_mutex);
			m_tasks.emplace ([_task] () { (*_task)(); });
			ul.unlock ();
			m_condition.notify_one ();
			return _res;
		}

	private:
		std::vector<std::thread>			m_workers;
		std::queue<std::function<void ()>>	m_tasks;
		std::mutex							m_mutex;
		std::condition_variable				m_condition;
		volatile bool						m_run = true;
	};
}



#endif //FAWLIB__THREAD_POOL_HPP__
