#include "pch.h"
#include <Winsock2.h>
#include "PlayerManager.h"
#include "RoomManager.h"

namespace GameServer
{
	namespace Entity
	{
		void PlayerManager::AddPlayer(std::string userId, std::string userName, std::string token, std::string experience, std::string money, std::string level, std::string curVehicleId, std::string registerTime, std::string lastLoginTime, vector<VehicleModel> vehicleList, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
			_onlinePlayerMap.emplace(std::piecewise_construct, std::make_tuple(userId), std::make_tuple(userId, userName, token, experience, money, level, curVehicleId, registerTime, lastLoginTime, vehicleList, connSocket, sendMessage));
		}
		void PlayerManager::RemovePlayer(std::string userId)
		{
			auto iter = _onlinePlayerMap.find(userId);
			if (iter != _onlinePlayerMap.end())
			{
				_onlinePlayerMap.erase(iter);
			}
		}
		PlayerModel* PlayerManager::GetPlayer(std::string userId)
		{
			auto iter = _onlinePlayerMap.find(userId);
			if (iter != _onlinePlayerMap.end())
			{
				return &iter->second;
			}
			return nullptr;
		}
		PlayerModel* PlayerManager::GetPlayerBySocket(unsigned __int64 connSocket)	//客户端意外断开时只能通过套接字来找到用户
		{
			for (auto& pair : _onlinePlayerMap)
			{
				if (pair.second.ConnSocket == connSocket)
				{
					return &pair.second;
				}
			}
			return nullptr;
		}
		PlayerManager::PlayerManager()
		{
		}


		PlayerManager::~PlayerManager()
		{
			_onlinePlayerMap.clear();
		}
	}
}
