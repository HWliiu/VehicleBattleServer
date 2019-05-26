#pragma once
#include <mutex>
#include "AccountHandle.h"
#include "StoreHandle.h"
#include "LobbyHandle.h"
#include "RoomHandle.h"
#include "Singleton.h"
#include "BattleHandle.h"

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
			inline BattleHandle* GetBattleHandle() { return &_battleHandle; }
		private:
			AccountHandle _accountHandle;
			StoreHandle _storeHandle;
			LobbyHandle _lobbyHandle;
			RoomHandle _roomHandle;
			BattleHandle _battleHandle;

			HandleManager()
			{
			}
			~HandleManager()
			{
			}
		};
	}
}

