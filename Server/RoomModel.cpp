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
		//����ture��ʾ����ɹ�,����false��ʾ������������
		bool RoomModel::AddPlayer(PlayerModel* player)
		{
			if (PlayerList.size() < 8)
			{
				//����player��room��صĲ���
				player->SetCurRoomId(RoomId);
				player->SetPrepareState(false);
				//if (RoomMode == "SingleMode")
				//{
				//	player->SetTeam("None");
				//}
				//else if (RoomMode == "TeamMode")
				//{
				//	if (PlayerList.size() < 4)	//�������
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
				if (PlayerList.size() > 1)	//����������ң�ת�÷���
				{
					PlayerList.remove(player);
					Owner = PlayerList.front();
					//֪ͨ�������
				}
				else	//û��������ң�ɾ������
				{
					PlayerList.remove(player);
					RoomManager::GetInstance()->RemoveRoom(RoomId);
				}
			}
			else
			{
				PlayerList.remove(player);
				//֪ͨ�������
			}
		}
	}
}
