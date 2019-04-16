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
			LoginHandle *GetLoginHandle();
		private:
			HandleManager();
			~HandleManager();

			LoginHandle* _pLoginHandle;
		};
	}
}

