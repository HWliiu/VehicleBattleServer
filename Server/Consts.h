#pragma once
#include <string>
namespace GameServer
{
	namespace Common
	{
		//Request
		const std::string RequestLogin = "RequestLogin";
		const std::string RequestRegister = "RequestRegister";
		const std::string RequestLogout = "RequestLogout";

		const std::string RequestChangePassword = "RequestChangePassword";
		const std::string RequestStoreItemList = "RequestStoreItemList";
		const std::string RequestPurchaseItem = "RequestPurchaseItem";
		const std::string RequestChangeVehicle = "RequestChangeVehicle";

		const std::string RequestCreateRoom = "RequestCreateRoom";
		const std::string RequestSearchRoom = "RequestSearchRoom";
		const std::string RequestJoinRoom = "RequestJoinRoom";
		const std::string RequestRefreshRoomList = "RequestRefreshRoomList";

		const std::string RequestExitRoom = "RequestExitRoom";
		const std::string RequestChangePrepareState = "RequestChangePrepareState";
		const std::string RequestStartGame = "RequestStartGame";
		const std::string RequestKickPlayer = "RequestKickPlayer";
		const std::string RequestSendMessage = "RequestSendMessage";

		const std::string UpLoadTransformState = "UpLoadTransformState";
		const std::string UpLoadFireState = "UpLoadFireState";
		const std::string UpLoadHealthState = "UpLoadHealthState";


		//Result
		const std::string LoginResult = "LoginResult";
		const std::string RegisterResult = "RegisterResult";
		const std::string LogoutResult = "LogoutResult";

		const std::string ChangePasswordResult = "ChangePasswordResult";
		const std::string PurchaseItemResult = "PurchaseItemResult";
		const std::string ChangeVehicleResult = "ChangeVehicleResult";
		const std::string StoreItemListResult = "StoreItemListResult";

		const std::string CreateRoomResult = "CreateRoomResult";
		const std::string SearchRoomResult = "SearchRoomResult";
		const std::string JoinRoomResult = "JoinRoomResult";
		const std::string RefreshRoomListResult = "RefreshRoomListResult";

		const std::string ExitRoomResult = "ExitRoomResult";
		const std::string ChangePrepareStateResult = "ChangePrepareStateResult";
		const std::string StartGameResult = "StartGameResult";
		const std::string KickPlayerResult = "KickPlayerResult";
		const std::string SendMessageResult = "SendMessageResult";

		const std::string UpdateTransformState = "UpdateTransformState";
		const std::string UpdateFireState = "UpdateFireState";
		const std::string UpdateHealthState = "UpdateHealthState";

		const std::string SUCCEED = "succeed";
		const std::string FAILURE = "failure";

#define CONSTRUCT_DOCUMENT(command) \
		Document document;\
		Pointer("/Command").Set(document, ##command);

#define SERIALIZE_DOCUMENT \
		StringBuffer buffer;\
		Writer<StringBuffer> writer(buffer);\
		document.Accept(writer);\
		const char* output = buffer.GetString();

#define HANDLE_CATCH(sendMessage) \
		catch (const std::exception & e)\
		{\
			printf("%s\n", e.what());\
			Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());\
			Pointer("/Paras/Info").Set(document, "·þÎñÆ÷Òì³£");\
			StringBuffer buffer;\
			Writer<StringBuffer> writer(buffer);\
			document.Accept(writer);\
			const char* output = buffer.GetString();\
			##sendMessage(output);\
		}
	}
}
