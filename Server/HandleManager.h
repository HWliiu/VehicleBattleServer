#pragma once
#include <mutex>
#include "AccountHandle.h"
#include "StoreHandle.h"
#include "LobbyHandle.h"
#include "RoomHandle.h"
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
			inline AccountHandle* GetAccountHandle() { return &_accountHandle; }
			inline StoreHandle* GetStoreHandle() { return &_storeHandle; }
			inline LobbyHandle* GetLobbyHandle() { return &_lobbyHandle; }
			inline RoomHandle* GetRoomHandle() { return &_roomHandle; }
		private:
			AccountHandle _accountHandle;
			StoreHandle _storeHandle;
			LobbyHandle _lobbyHandle;
			RoomHandle _roomHandle;

			HandleManager()
			{
			}
			~HandleManager()
			{
			}
		};
	}
}

