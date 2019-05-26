#pragma once
#include <iostream>
#include "PlayerModel.h"
#include "RoomModel.h"

namespace GameServer
{
	using namespace Entity;
	namespace Handle
	{
		class LobbyHandle
		{
		public:
			void CreateRoom(string userId, string roomName, string roomMode, string roomMap, PlayerModel* player);
			void RefreshRoomList(string userId, PlayerModel* player);
			void JoinRoom(string userId, string roomId, PlayerModel* player);
			void SearchRoom(string userId, string roomId, PlayerModel* player);
		};
	}
}

