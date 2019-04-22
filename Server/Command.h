#pragma once
#include "include/rapidjson/document.h"
#include <functional>

using namespace rapidjson;

namespace GameServer
{
	namespace Command
	{
		class ICommand
		{
		public:
			virtual void Execute(Document document, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage) = 0;
		};

		class LoginCommand :public ICommand
		{
		public:
			virtual void Execute(Document document, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage) override;
		};

		class RegisterCommand :public ICommand
		{
		public:
			virtual void Execute(Document document, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage) override;
		};

		class LogoutCommand :public ICommand
		{
		public:
			virtual void Execute(Document document, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage) override;
		};

		class ChangePasswordCommand :public ICommand
		{
		public:
			virtual void Execute(Document document, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage) override;
		};

	}
}

