#include "pch.h"
#include "Command.h"
#include "HandleFactory.h"


namespace GameServer
{
	using namespace Handle;
	namespace Command
	{
		void LoginCommand::Execute(Document document, std::function<void(std::string)> sendMessage)
		{
			rapidjson::Value* username = Pointer("/Paras/Username").Get(document);
			rapidjson::Value* password = Pointer("/Paras/Password").Get(document);

			auto pLoginHandle = HandleFactory::GetInstance()->GetLoginHandle();
			pLoginHandle->Login(username->GetString(), password->GetString(), sendMessage);
		}
		void RegisterCommand::Execute(Document document, std::function<void(std::string)> sendMessage)
		{
			rapidjson::Value* username = Pointer("/Paras/Username").Get(document);
			rapidjson::Value* password = Pointer("/Paras/Password").Get(document);

			auto pLoginHandle = HandleFactory::GetInstance()->GetLoginHandle();
			pLoginHandle->Register(username->GetString(), password->GetString(), sendMessage);
		}
	}
}

