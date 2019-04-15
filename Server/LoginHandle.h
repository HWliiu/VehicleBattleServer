#pragma once
#include <iostream>
#include <functional>
#include "include/rapidjson/document.h"
#include "include/rapidjson/pointer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/writer.h"
#include "DbUtil.h"

namespace GameServer
{
	using namespace Util;
	using namespace rapidjson;
	namespace Handle
	{
		//using std::string;	//rapidjson和mysqlx内部都有自己string的定义,要用std的string时统一用std::string,不要图方便用这个using
		class LoginHandle
		{
		public:
			void Login(std::string username, std::string password, std::function<void(std::string)> sendMessage);
			void Logout(std::string username, std::function<void(std::string)> sendMessage);
			void Register(std::string username, std::string password, std::function<void(std::string)> sendMessage);
		};
	}
}

