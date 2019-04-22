#pragma once
#include <iostream>
#include <functional>

namespace GameServer
{
	namespace Handle
	{
		using std::string;
		class AccountHandle
		{
		public:
			void Login(string username, string password, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage);
			void Logout(string userId, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage);
			void Register(string username, string password, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage);
			void ChangePassword(string userId, string oldPassword, string newPassword, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage);
		};
	}
}

