#pragma once
#include <functional> 
#include "include/rapidjson/document.h"
#include "include/rapidjson/pointer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/writer.h"
#include "HandleFactory.h"

using namespace rapidjson;

namespace GameServer
{
	namespace Command
	{
		class ICommand
		{
		public:
			virtual void Execute(Document document, std::function<void(std::string)> sendMessage) = 0;
		};

		class LoginCommand :public ICommand
		{
		public:

			// ͨ�� ICommand �̳�
			virtual void Execute(Document document, std::function<void(std::string)> sendMessage) override;
		};

		class RegisterCommand :public ICommand
		{
		public:

			// ͨ�� ICommand �̳�
			virtual void Execute(Document document, std::function<void(std::string)> sendMessage) override;
		};


	}
}

