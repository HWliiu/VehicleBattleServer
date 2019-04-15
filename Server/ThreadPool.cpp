#include "pch.h"
#include "ThreadPool.h"

namespace GameServer
{
	namespace Util
	{
		// the constructor just launches some amount of workers
		ThreadPool::ThreadPool(size_t threads) : _stop(false)
		{
			for (size_t i = 0; i < threads; ++i)
			{
				_workers.emplace_back([this]
					{
						while (true)
						{
							std::function<void()> task;   //�߳��еĺ�������
							{
								std::unique_lock<std::mutex> lock(this->_queueMtx);
								this->_condition.wait(lock, [this] { return this->_stop || !this->_tasks.empty(); }); //��stop==false&&tasks.empty(),���̱߳�����
								if (this->_stop && this->_tasks.empty())
								{
									return;
								}
								task = std::move(this->_tasks.front());
								this->_tasks.pop();
							}
							task(); //���ú��������к���
						}
					});
			}
		}

		// add new work item to the pool
		template<class F, class... Args>
		auto ThreadPool::Enqueue(F&& f, Args&& ... args) -> std::future<typename std::result_of<F(Args...)>::type>
		{
			using return_type = typename std::result_of<F(Args...)>::type;
			//packaged_task�Ƕ������һ���������ǿ��Ը��䴫��һ������������乹�졣֮������Ͷ�ݸ��κ��߳�ȥ��ɣ�ͨ��packaged_task.get_future()������ȡ��future����ȡ������ɺ�Ĳ���ֵ
			auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));	//std::forward��������ת��
			//futureΪ������get_future��ȡ������ɺ�Ĳ���ֵ
			std::future<return_type> res = task->get_future();   //��ȡfuture�������task��״̬��Ϊready����������ǰ������
			{
				std::unique_lock<std::mutex> lock(_queueMtx);
				// don't allow enqueueing after stopping the pool
				if (_stop)
				{
					throw std::runtime_error("Enqueue on stopped ThreadPool");
				}
				_tasks.emplace([task]() { (*task)(); });  //��taskͶ�ݸ��߳�ȥ���
			}
			_condition.notify_one();  //ѡ��һ��wait״̬���߳̽��л��ѣ���ʹ����ö����ϵ������������(�������߳��޷����ʶ���)
			return res;
		}//notify_one���ܱ�֤��������߳�������Ҫ����������˿��ܲ�������

		// the destructor joins all threads
		ThreadPool::~ThreadPool()
		{
			{
				std::unique_lock<std::mutex> lock(_queueMtx);
				_stop = true;
			}
			_condition.notify_all();  //֪ͨ����wait״̬���߳̾�������Ŀ���Ȩ�����������߳�ִ��
			for (std::thread& worker : _workers)
			{
				worker.join(); //��Ϊ�̶߳���ʼ�����ˣ�����һ����ִ���꣬join�ɵȴ��߳�ִ����
			}
		}
	}
}
