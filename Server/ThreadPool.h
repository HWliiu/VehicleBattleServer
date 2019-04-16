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
			auto Enqueue(F&& f, Args&& ... args)->std::future<decltype(f(args...))>;  //std::future������ȡ�첽����Ľ��
			~ThreadPool();
		private:
			// need to keep track of threads so we can join them
			std::vector<std::thread> _workers;
			// the task queue
			std::queue<std::function<void()>> _tasks;    //������У����ڴ��û�д���������ṩ�������
			std::mutex _queueMtx;
			std::condition_variable _condition;	//��������
			bool _stop;
		};
	}
}

