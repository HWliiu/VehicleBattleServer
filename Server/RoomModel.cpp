#include "pch.h"
#include "RoomModel.h"
#include "RoomManager.h"

namespace GameServer
{
	namespace Entity
	{
		RoomModel::RoomModel(std::string roomId, std::string roomName, std::string roomMode, std::string roomMap, PlayerModel* owner) :RoomId(roomId), RoomName(roomName)
		{
			RoomMode = roomMode;
			RoomMap = roomMap;
			Owner = owner;
			IsGaming = false;
			AddPlayer(Owner);
		}

		RoomModel::~RoomModel()
		{
		}
		//返回ture表示加入成功,返回false表示房间人数满了
		bool RoomModel::AddPlayer(PlayerModel* player)
		{
			if (PlayerList.size() < 8)
			{
				//设置player与room相关的参数
				player->SetCurRoomId(RoomId);
				player->SetPrepareState(false);
				//if (RoomMode == "SingleMode")
				//{
				//	player->SetTeam("None");
				//}
				//else if (RoomMode == "TeamMode")
				//{
				//	if (PlayerList.size() < 4)	//分配队伍
				//	{
				//		player->SetTeam("Red");
				//	}
				//	else
				//	{
				//		player->SetTeam("Blue");
				//	}
				//}

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
				if (PlayerList.size() > 1)	//还有其他玩家，转让房主
				{
					PlayerList.remove(player);
					Owner = PlayerList.front();
					//通知其他玩家
				}
				else	//没有其他玩家，删除房间
				{
					PlayerList.remove(player);
					RoomManager::GetInstance()->RemoveRoom(RoomId);
				}
			}
			else
			{
				PlayerList.remove(player);
				//通知其他玩家
			}
		}
	}
}
