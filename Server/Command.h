#pragma once
#include <functional> 
#include "include/rapidjson/document.h"

using namespace rapidjson;

namespace GameServer
{
	namespace Command
	{
		class ICommand
		{
		public:
			virtual void Execute(Document document, std::function<void(std::string)> sendMessage) = 0;	//Document实现了移动语义
		};

		class LoginCommand :public ICommand
		{
		public:

			// 通过 ICommand 继承
			virtual void Execute(Document document, std::function<void(std::string)> sendMessage) override;
		};

		class RegisterCommand :public ICommand
		{
		public:

			// 通过 ICommand 继承
			virtual void Execute(Document document, std::function<void(std::string)> sendMessage) override;
		};


	}
}

