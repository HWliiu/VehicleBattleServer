#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

namespace GameServer
{
	namespace Util
	{
		class ThreadPool
		{
		public:
			ThreadPool(size_t);
			template<class F, class... Args>
			auto Enqueue(F&& f, Args&& ... args)->std::future<decltype(f(args...))>;  //std::future用来获取异步任务的结果
			~ThreadPool();
		private:
			// need to keep track of threads so we can join them
			std::vector<std::thread> _workers;
			// the task queue
			std::queue<std::function<void()>> _tasks;    //任务队列，用于存放没有处理的任务。提供缓冲机制
			std::mutex _queueMtx;
			std::condition_variable _condition;	//条件阻塞
			bool _stop;
		};
	}
}

