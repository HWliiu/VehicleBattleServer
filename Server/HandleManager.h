#pragma once
#include <mutex>
#include "LoginHandle.h"
#include "Singleton.h"

namespace GameServer
{
	using namespace Util;
	namespace Handle
	{
		class HandleManager :public Singleton<HandleManager>
		{
		public:
			friend class Singleton<HandleManager>;
			inline LoginHandle* GetLoginHandle() { return _pLoginHandle; }
		private:
			HandleManager()
			{
				_pLoginHandle = new LoginHandle();
			}
			~HandleManager()
			{
				delete _pLoginHandle;
				_pLoginHandle = nullptr;
			}

			LoginHandle* _pLoginHandle;
		};
	}
}

