#include "pch.h"
#include "HandleFactory.h"

namespace GameServer
{
	namespace Handle
	{
		HandleFactory* HandleFactory::_instance;
		std::mutex HandleFactory::_mtx;

		HandleFactory::HandleFactory()
		{
			_pLoginHandle = new LoginHandle();
		}

		HandleFactory::~HandleFactory()
		{
			delete _pLoginHandle;
			_pLoginHandle = nullptr;
		}

		HandleFactory* HandleFactory::GetInstance()
		{
			if (_instance == nullptr)
			{
				std::lock_guard<std::mutex> lock(_mtx);
				if (_instance == nullptr)
				{
					_instance = new HandleFactory();
				}
			}
			return _instance;
		}
		LoginHandle* HandleFactory::GetLoginHandle()
		{
			return _pLoginHandle;
		}
	}
}
