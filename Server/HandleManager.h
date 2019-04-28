#pragma once
#include <mutex>
#include "AccountHandle.h"
#include "StoreHandle.h"
#include "Singleton.h"

namespace GameServer
{
	using namespace Common;
	namespace Handle
	{
		class HandleManager :public Singleton<HandleManager>
		{
		public:
			friend class Singleton<HandleManager>;
			inline AccountHandle* GetAccountHandle() { return &accountHandle; }
			inline StoreHandle* GetStoreHandle() { return &storeHandle; }
		private:
			AccountHandle accountHandle;
			StoreHandle storeHandle;

			HandleManager()
			{
			}
			~HandleManager()
			{
			}
		};
	}
}

