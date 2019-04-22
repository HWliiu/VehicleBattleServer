#pragma once
#include <mutex>
#include "AccountHandle.h"
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
			inline AccountHandle* GetAccountHandle() { return _pAccountHandle; }
		private:
			HandleManager()
			{
				_pAccountHandle = new AccountHandle();
			}
			~HandleManager()
			{
				delete _pAccountHandle;
				_pAccountHandle = nullptr;
			}

			AccountHandle* _pAccountHandle;
		};
	}
}

