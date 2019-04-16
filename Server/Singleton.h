#pragma once
#include <mutex>

namespace GameServer
{
	namespace Util
	{
		//使用方法:子类继承Singleton<T>并将Singleton<T>设为友元类
		template <class T>
		class Singleton
		{
		private:
			static T* _instance;
			static std::mutex _mtx;

			//禁止拷贝构造和赋值运算符. The only way is getInstance()
			Singleton(const Singleton&) = delete;
			Singleton& operator=(const Singleton&) = delete;

			class Garbo   //它的唯一工作就是在析构函数中析构Singleton的实例，所以private
			{
			public:
				~Garbo()
				{
					if (Singleton::_instance)
					{
						delete Singleton::_instance;
						Singleton::_instance = nullptr;
					}
				}
			};
			static Garbo garbo;  //定义一个静态成员变量，程序结束时，系统会自动调用它的析构函数，我们不需要访问这个变量，所以不需要初始化

		protected:
			//使继承者无法public构造函数和析构函数
			Singleton() {}
			~Singleton() {}

		public:
			static T* GetInstance()
			{
				if (_instance == nullptr)
				{
					std::lock_guard<std::mutex> lock(_mtx);
					if (_instance == nullptr)
					{
						_instance = new T();
					}
				}
				return _instance;
			}
		};
		//初始化静态成员，初始化的过程中会分配内存
		template <class T>
		T* Singleton<T>::_instance = nullptr;
		template <class T>
		std::mutex Singleton<T>::_mtx;
	}
}

