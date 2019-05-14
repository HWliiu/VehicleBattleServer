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
			void ChangePrepareState(string userId, bool prepareState, PlayerModel* player);
			void KickPlayer(string userId, string playerId, PlayerModel* player);
			void SendMsg(string userId, string message, PlayerModel* player);
			void StartGame(string userId, PlayerModel* player);

		};
	}
}
