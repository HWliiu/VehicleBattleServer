#include "pch.h"
#include "RoomModel.h"

namespace GameServer
{
	namespace Entity
	{
		RoomModel::RoomModel(std::string roomId, PlayerModel* owner) :RoomId(roomId)
		{
			Owner = owner;
			PlayerList.push_back(Owner);
			CanStartGame = false;
		}

		RoomModel::~RoomModel()
		{
		}
		bool RoomModel::AddPlayer(PlayerModel* player)
		{
			if (PlayerList.size() < 10)
			{
				PlayerList.push_back(player);
				return true;
			}
			else
			{
				return false;
			}
		}
		void RoomModel::RemovePlayer(PlayerModel* player)
		{
			if (player != Owner)
			{
			}
		}
	}
}
