#include "pch.h"
#include "Command.h"
#include "include/rapidjson/pointer.h"
#include "HandleManager.h"
#include "PlayerManager.h"

namespace GameServer
{
	using namespace Handle;
	namespace Command
	{
		void LoginCommand::Execute(Document document, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
			auto username = Pointer("/Paras/UserName").Get(document)->GetString();
			auto password = Pointer("/Paras/Password").Get(document)->GetString();

			auto pAccountHandle = HandleManager::GetInstance()->GetAccountHandle();
			pAccountHandle->Login(username, password, connSocket, sendMessage);
		}
		void RegisterCommand::Execute(Document document, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
			auto username = Pointer("/Paras/UserName").Get(document)->GetString();
			auto password = Pointer("/Paras/Password").Get(document)->GetString();

			auto pAccountHandle = HandleManager::GetInstance()->GetAccountHandle();
			pAccountHandle->Register(username, password, connSocket, sendMessage);
		}
		void LogoutCommand::Execute(Document document, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
			auto userId = Pointer("/Paras/UserId").Get(document)->GetString();
			auto token = Pointer("/Paras/Token").Get(document)->GetString();

			auto player = Entity::PlayerManager::GetInstance()->GetPlayer(userId);
			if (player == nullptr || player->GetToken() != token)	//简单的验证,防止篡改其他玩家的数据(直接对比套接字也行)
			{
				printf("para error\n");
				return;
			}

			auto pAccountHandle = HandleManager::GetInstance()->GetAccountHandle();
			pAccountHandle->Logout(userId, connSocket, sendMessage);
		}
		void ChangePasswordCommand::Execute(Document document, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
			auto userId = Pointer("/Paras/UserId").Get(document)->GetString();
			auto token = Pointer("/Paras/Token").Get(document)->GetString();
			auto oldPassword = Pointer("/Paras/OldPassword").Get(document)->GetString();
			auto newPassword = Pointer("/Paras/NewPassword").Get(document)->GetString();

			auto player = Entity::PlayerManager::GetInstance()->GetPlayer(userId);
			if (player == nullptr || player->GetToken() != token)
			{
				printf("para error\n");
				return;
			}

			auto pAccountHandle = HandleManager::GetInstance()->GetAccountHandle();
			pAccountHandle->ChangePassword(userId, oldPassword, newPassword, connSocket, sendMessage);
		}
	}
}
