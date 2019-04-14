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
		using std::string;
		class LoginHandle
		{
		public:
			void Login(string username, string password, std::function<void(string)> sendMessage);
			void Logout(string username, std::function<void(std::string)> sendMessage);
			void Register(string username, string password, std::function<void(string)> sendMessage);
		};
	}
}

