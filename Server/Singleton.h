#pragma once
#include <mutex>

namespace GameServer
{
	namespace Util
	{
		//ʹ�÷���:����̳�Singleton<T>����Singleton<T>��Ϊ��Ԫ��
		template <class T>
		class Singleton
		{
		private:
			static T* _instance;
			static std::mutex _mtx;

			//��ֹ��������͸�ֵ�����. The only way is getInstance()
			Singleton(const Singleton&) = delete;
			Singleton& operator=(const Singleton&) = delete;

			class Garbo   //����Ψһ������������������������Singleton��ʵ��������private
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
			static Garbo garbo;  //����һ����̬��Ա�������������ʱ��ϵͳ���Զ����������������������ǲ���Ҫ����������������Բ���Ҫ��ʼ��

		protected:
			//ʹ�̳����޷�public���캯������������
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
		//��ʼ����̬��Ա����ʼ���Ĺ����л�����ڴ�
		template <class T>
		T* Singleton<T>::_instance = nullptr;
		template <class T>
		std::mutex Singleton<T>::_mtx;
	}
}

