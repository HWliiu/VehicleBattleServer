#include "pch.h"
#include "HandleManager.h"

namespace GameServer
{
	namespace Handle
	{
		HandleManager::HandleManager()
		{
			_pLoginHandle = new LoginHandle();
		}

		HandleManager::~HandleManager()
		{
			delete _pLoginHandle;
			_pLoginHandle = nullptr;
		}

		LoginHandle* HandleManager::GetLoginHandle()
		{
			return _pLoginHandle;
		}
	}
}
