#pragma once
#include <iostream>
#include "PlayerModel.h"

namespace GameServer
{
	using namespace Entity;
	namespace Handle
	{
		class RoomHandle
		{
		public:
			void ExitRoom(string userId, PlayerModel* player);
			void KickPlayer(string userId, string roomId, string playerId, PlayerModel* player);
			void SendMsg(string userId, string roomId, PlayerModel* player);
			void StartGame(string userId, string roomId, PlayerModel* player);

		};
	}
}
