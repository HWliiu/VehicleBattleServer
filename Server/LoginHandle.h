#pragma once
#include <iostream>
#include <functional>

namespace GameServer
{
	namespace Handle
	{
		//using std::string;	//rapidjson��mysqlx�ڲ������Լ�string�Ķ���,Ҫ��std��stringʱͳһ��std::string,��Ҫͼ���������using
		class LoginHandle
		{
		public:
			void Login(std::string username, std::string password, std::function<void(std::string)> sendMessage);
			void Logout(std::string username, std::function<void(std::string)> sendMessage);
			void Register(std::string username, std::string password, std::function<void(std::string)> sendMessage);
		};
	}
}

