#pragma once
#include <list>
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
			const std::string RoomName;
			PlayerModel* Owner;
			std::string RoomMode;
			std::string RoomMap;

			list<PlayerModel*> PlayerList;

			RoomModel(std::string roomId, std::string roomName, std::string roomMode, std::string roomMap, PlayerModel* owner);
			~RoomModel();
			bool AddPlayer(PlayerModel* player);
			void RemovePlayer(PlayerModel* player);
		};
	}
}

