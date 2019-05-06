#include "pch.h"
#include "RoomModel.h"

namespace GameServer
{
	namespace Entity
	{
		RoomModel::RoomModel(std::string roomId, std::string roomName, std::string roomMode, std::string roomMap, PlayerModel* owner) :RoomId(roomId), RoomName(roomName)
		{
			RoomMode = roomMode;
			RoomMap = roomMap;
			Owner = owner;
			AddPlayer(Owner);
		}

		RoomModel::~RoomModel()
		{
		}
		bool RoomModel::AddPlayer(PlayerModel* player)
		{
			if (PlayerList.size() < 8)
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
			if (player == Owner)
			{
				PlayerList.remove(player);
				Owner = PlayerList.front();
			}
			else
			{
				PlayerList.remove(player);
				//通知其他玩家
			}
		}
	}
}
