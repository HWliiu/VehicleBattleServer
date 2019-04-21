#include "pch.h"
#include "Command.h"
#include "include/rapidjson/pointer.h"
#include "HandleManager.h"

namespace GameServer
{
	using namespace Handle;
	namespace Command
	{
		void LoginCommand::Execute(Document document, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
			rapidjson::Value* username = Pointer("/Paras/UserName").Get(document);
			rapidjson::Value* password = Pointer("/Paras/Password").Get(document);

			auto pLoginHandle = HandleManager::GetInstance()->GetLoginHandle();
			pLoginHandle->Login(username->GetString(), password->GetString(), connSocket, sendMessage);
		}
		void RegisterCommand::Execute(Document document, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
			rapidjson::Value* username = Pointer("/Paras/UserName").Get(document);
			rapidjson::Value* password = Pointer("/Paras/Password").Get(document);

			auto pLoginHandle = HandleManager::GetInstance()->GetLoginHandle();
			pLoginHandle->Register(username->GetString(), password->GetString(), connSocket, sendMessage);
		}
		void LogoutCommand::Execute(Document document, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
		}
	}
}
