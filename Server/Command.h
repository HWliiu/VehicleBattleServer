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
			virtual void Execute(Document document, std::function<void(std::string)> sendMessage) = 0;	//Documentʵ�����ƶ�����
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

