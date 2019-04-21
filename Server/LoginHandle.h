#pragma once
#include <iostream>
#include <functional>

namespace GameServer
{
	namespace Handle
	{
		using std::string;
		class LoginHandle
		{
		public:
			void Login(string username, string password, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage);
			void Logout(string userId, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage);
			void Register(string username, string password, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage);
		};
	}
}

