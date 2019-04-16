#include "pch.h"
#include "ThreadPool.h"

namespace GameServer
{
	namespace Util
	{
		// the constructor just launches some amount of workers
		ThreadPool::ThreadPool(size_t threads) : _stop(false)
		{
			threads = threads < 1 ? 1 : threads;
			for (size_t i = 0; i < threads; ++i)
			{
				_workers.emplace_back([this]
					{
						while (!_stop)
						{
							std::function<void()> task;   //线程中的函数对象
							{
								std::unique_lock<std::mutex> lock(this->_queueMtx);
								this->_condition.wait(lock, [this] { return this->_stop || !this->_tasks.empty(); });
								if (this->_stop && this->_tasks.empty())
								{
									return;
								}
								task = std::move(this->_tasks.front());
								this->_tasks.pop();
							}
							task(); //调用函数，运行函数
						}
					});
			}
		}

		// add new work item to the pool
		template<class F, class... Args>
		auto ThreadPool::Enqueue(F&& f, Args&& ... args) -> std::future<decltype(f(args...))>	//后置返回类型
		{
			using return_type = decltype(f(args...)); //typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型
			//packaged_task是对任务的一个抽象，我们可以给其传递一个函数来完成其构造。之后将任务投递给任何线程去完成，通过packaged_task.get_future()方法获取的future来获取任务完成后的产出值
			auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));	//std::forward进行完美转发
			//future为期望，get_future获取任务完成后的产出值
			std::future<return_type> res = task->get_future();   //获取future对象，如果task的状态不为ready，会阻塞当前调用者
			{
				std::unique_lock<std::mutex> lock(_queueMtx);
				// don't allow enqueueing after stopping the pool
				if (_stop)
				{
					throw std::runtime_error("Enqueue on stopped ThreadPool");
				}
				_tasks.emplace([task]() { (*task)(); });  //将task投递给线程去完成
			}
			_condition.notify_one();  //唤醒一个线程执行
			return res;
		}

		// the destructor joins all threads
		ThreadPool::~ThreadPool()
		{
			{
				std::unique_lock<std::mutex> lock(_queueMtx);
				_stop = true;
			}
			_condition.notify_all();  //唤醒所有线程执行
			for (std::thread& worker : _workers)
			{
				worker.join(); //等待任务结束， 前提：线程一定会执行完
			}
		}
	}
}
