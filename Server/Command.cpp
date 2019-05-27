#include "pch.h"
#include "Command.h"
#include "include/rapidjson/pointer.h"
#include "HandleManager.h"
#include "PlayerManager.h"

//简单的验证,防止篡改其他玩家的数据(直接对比套接字也行)
#define VERIFY_ACCOUNT \
	auto userId = Pointer("/Paras/UserId").Get(document)->GetString();\
	auto token = Pointer("/Paras/Token").Get(document)->GetString();\
	auto player = Entity::PlayerManager::GetInstance()->GetPlayer(userId);\
	if (player == nullptr || player->GetToken() != token)\
	{\
		printf("para error\n");\
		return;\
	}

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
		void RegisterCommand::Execute(Document document, std::function<void(std::string)> sendMessage)
		{
			auto username = Pointer("/Paras/UserName").Get(document)->GetString();
			auto password = Pointer("/Paras/Password").Get(document)->GetString();

			auto pAccountHandle = HandleManager::GetInstance()->GetAccountHandle();
			pAccountHandle->Register(username, password, sendMessage);
		}
		void LogoutCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;	//(最后这个分号用来纠正格式化代码)

			auto pAccountHandle = HandleManager::GetInstance()->GetAccountHandle();
			pAccountHandle->Logout(userId, player);
		}
		void ChangePasswordCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			auto oldPassword = Pointer("/Paras/OldPassword").Get(document)->GetString();
			auto newPassword = Pointer("/Paras/NewPassword").Get(document)->GetString();

			auto pAccountHandle = HandleManager::GetInstance()->GetAccountHandle();
			pAccountHandle->ChangePassword(userId, oldPassword, newPassword, player);
		}

		void StoreItemListCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			auto pStoreHandle = HandleManager::GetInstance()->GetStoreHandle();
			pStoreHandle->ListStoreItem(userId, player);
		}
		void PurchaseCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			auto vehicleId = Pointer("/Paras/VehicleId").Get(document)->GetString();
			auto pAccountHandle = HandleManager::GetInstance()->GetAccountHandle();
			pAccountHandle->PurchaseVehicle(userId, vehicleId, player);
		}

		void ChangeVehicleCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			auto vehicleId = Pointer("/Paras/VehicleId").Get(document)->GetString();
			auto pAccountHandle = HandleManager::GetInstance()->GetAccountHandle();
			pAccountHandle->ChangeVehicle(userId, vehicleId, player);
		}

		void CreateRoomCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			auto roomName = Pointer("/Paras/RoomName").Get(document)->GetString();
			auto roomMode = Pointer("/Paras/RoomMode").Get(document)->GetString();
			auto roomMap = Pointer("/Paras/RoomMap").Get(document)->GetString();

			auto pLobbyHandle = HandleManager::GetInstance()->GetLobbyHandle();
			pLobbyHandle->CreateRoom(userId, roomName, roomMode, roomMap, player);
		}
		void RefreshRoomListCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			auto pLobbyHandle = HandleManager::GetInstance()->GetLobbyHandle();
			pLobbyHandle->RefreshRoomList(userId, player);
		}
		void JoinRoomCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			auto roomId = Pointer("/Paras/RoomId").Get(document)->GetString();

			auto pLobbyHandle = HandleManager::GetInstance()->GetLobbyHandle();
			pLobbyHandle->JoinRoom(userId, roomId, player);
		}
		void SearchRoomCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			auto roomId = Pointer("/Paras/RoomId").Get(document)->GetString();

			auto pLobbyHandle = HandleManager::GetInstance()->GetLobbyHandle();
			pLobbyHandle->SearchRoom(userId, roomId, player);
		}

		void ExitRoomCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			auto pRoomHandle = HandleManager::GetInstance()->GetRoomHandle();
			pRoomHandle->ExitRoom(userId, player);
		}
		void ChangePrepareStateCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			auto prepareState = Pointer("/Paras/PrepareState").Get(document)->GetBool();

			auto pRoomHandle = HandleManager::GetInstance()->GetRoomHandle();
			pRoomHandle->ChangePrepareState(userId, prepareState, player);
		}
		void KickPlayerCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			auto playerId = Pointer("/Paras/PlayerId").Get(document)->GetString();

			auto pRoomHandle = HandleManager::GetInstance()->GetRoomHandle();
			pRoomHandle->KickPlayer(userId, playerId, player);
		}
		void SendMessageCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			auto message = Pointer("/Paras/Message").Get(document)->GetString();

			auto pRoomHandle = HandleManager::GetInstance()->GetRoomHandle();
			pRoomHandle->SendMsg(userId, message, player);
		}
		void StartGameCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			auto pRoomHandle = HandleManager::GetInstance()->GetRoomHandle();
			pRoomHandle->StartGame(userId, player);
		}

		void UpLoadTransformStateCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			if (!Pointer("/Paras/Token").Erase(document)) return;

			auto pBattleHandle = HandleManager::GetInstance()->GetBattleHandle();
			pBattleHandle->UpdateTransformState(userId, std::move(document), player);
		}
		void UpLoadFireStateCommand::Execute(Document document)
		{
			VERIFY_ACCOUNT;

			if (!Pointer("/Paras/Token").Erase(document)) return;

			auto pBattleHandle = HandleManager::GetInstance()->GetBattleHandle();
			pBattleHandle->UpdateFireState(userId, std::move(document), player);
		}
		void UpLoadHealthStateCommand::Execute(Document document)
		{
		}
	}
}
