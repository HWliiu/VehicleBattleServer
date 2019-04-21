#pragma once
#include <vector>
#include <string>
#include "PlayerModel.h"

namespace GameServer
{
	namespace Entity
	{
		class RoomModel
		{
		public:
			const std::string RoomId;
			PlayerModel* Owner;
			vector<PlayerModel*> PlayerList;
			bool CanStartGame;

			RoomModel(std::string roomId, PlayerModel* owner);
			~RoomModel();
			bool AddPlayer(PlayerModel* player);
			void RemovePlayer(PlayerModel* player);
			void NotifyPlayer();
		};
	}
}

