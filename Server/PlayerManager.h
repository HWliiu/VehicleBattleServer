#pragma once
#include <unordered_map>
#include "Singleton.h"
#include "PlayerModel.h"

namespace GameServer
{
	using namespace Common;
	namespace Entity
	{
		class PlayerManager :public Singleton<PlayerManager>
		{
		public:
			friend class Singleton<PlayerManager>;
			void AddPlayer(std::string userId, std::string userName, std::string token, std::string experience, std::string money, std::string level, std::string curVehicleId, std::string registerTime, std::string lastLoginTime, vector<VehicleModel> vehicleList, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage);	//PlayerModel的构造函数的参数
			void RemovePlayer(std::string userId);
			PlayerModel* GetPlayer(std::string userId);
			PlayerModel* GetPlayerBySocket(unsigned __int64 connSocket);
		private:
			std::unordered_map<std::string, PlayerModel> _onlinePlayerMap;
			PlayerManager();
			~PlayerManager();
		};
	}
}

