#pragma once
#include "LoginHandle.h"
#include <mutex>

namespace GameServer
{
	namespace Handle
	{
		class HandleFactory
		{
		public:
			static HandleFactory* GetInstance();
			LoginHandle *GetLoginHandle();
		private:
			static HandleFactory* _instance;
			static std::mutex _mtx;
			HandleFactory();
			~HandleFactory();

			LoginHandle* _pLoginHandle;
		};
	}
}

