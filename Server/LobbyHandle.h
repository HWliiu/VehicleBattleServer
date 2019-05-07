#pragma once
#include <iostream>
#include <functional>
#include "PlayerModel.h"

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
		};
	}
}

