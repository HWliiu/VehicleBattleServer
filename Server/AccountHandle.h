#pragma once
#include <iostream>
#include <functional>
#include "PlayerModel.h"

namespace GameServer
{
	using namespace Entity;
	namespace Handle
	{
		using std::string;
		class AccountHandle
		{
		public:
			void Login(string username, string password, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage);
			void Register(string username, string password, std::function<void(std::string)> sendMessage);

			void Logout(string userId, PlayerModel *player, bool normalLogout = true);
			void ChangePassword(string userId, string oldPassword, string newPassword, PlayerModel* player);
			void PurchaseVehicle(string userId, string vehicleId, PlayerModel* player);
			void ChangeVehicle(string userId, string vehicleId, PlayerModel* player);
		};
	}
}

