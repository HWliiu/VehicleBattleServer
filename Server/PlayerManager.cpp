#include "pch.h"
#include <Winsock2.h>
#include "PlayerManager.h"
#include "RoomManager.h"

namespace GameServer
{
	namespace Entity
	{
		void PlayerManager::AddPlayer(std::string userId, std::string userName, std::string token, std::string level, std::string curVehicleId, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
			_onlinePlayerMap[userId] = new PlayerModel(userId, userName, token, level, curVehicleId, connSocket, sendMessage);
		}
		void PlayerManager::RemovePlayer(std::string userId)
		{
			auto iter = _onlinePlayerMap.find(userId);
			if (iter != _onlinePlayerMap.end())
			{
				//通知房间删除玩家
				auto roomId = GetPlayer(userId)->GetCurRoomId();
				if (roomId != "0")
				{
					RoomManager::GetInstance()->GetRoom(roomId)->RemovePlayer(GetPlayer(userId));
				}

				//closesocket(iter->second->ConnSocket);
				delete iter->second;
				iter->second = nullptr;
				_onlinePlayerMap.erase(iter);
			}
		}
		PlayerModel* PlayerManager::GetPlayer(std::string userId)
		{
			auto iter = _onlinePlayerMap.find(userId);
			if (iter != _onlinePlayerMap.end())
			{
				return iter->second;
			}
			return nullptr;
		}
		PlayerModel* PlayerManager::GetPlayerBySocket(unsigned __int64 connSocket)	//客户端意外断开时只能通过套接字来找到用户
		{
			for (auto iter = _onlinePlayerMap.begin(); iter != _onlinePlayerMap.end(); iter++)
			{
				if (iter->second->ConnSocket == connSocket)
				{
					return iter->second;
				}
			}
			return nullptr;
		}
		PlayerManager::PlayerManager()
		{
		}


		PlayerManager::~PlayerManager()
		{
			for (auto iter = _onlinePlayerMap.begin(); iter != _onlinePlayerMap.end(); iter++)
			{
				delete iter->second;
				iter->second = nullptr;
			}
			_onlinePlayerMap.clear();
		}
	}
}
