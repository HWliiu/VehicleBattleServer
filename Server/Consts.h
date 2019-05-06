#pragma once
#include <string>
namespace GameServer
{
	namespace Common
	{
		const std::string RequestLogin = "RequestLogin";
		const std::string RequestRegister = "RequestRegister";
		const std::string RequestLogout = "RequestLogout";
		const std::string RequestChangePassword = "RequestChangePassword";
		const std::string RequestStoreItemList = "RequestStoreItemList";
		const std::string RequestPurchaseItem = "RequestPurchaseItem";
		const std::string RequestChangeVehicle = "RequestChangeVehicle";
		const std::string RequestCreateRoom= "RequestCreateRoom";


		const std::string LoginResult = "LoginResult";
		const std::string RegisterResult = "RegisterResult";
		const std::string LogoutResult = "LogoutResult";
		const std::string ChangePasswordResult = "ChangePasswordResult";
		const std::string PurchaseItemResult = "PurchaseItemResult";
		const std::string ChangeVehicleResult = "ChangeVehicleResult";
		const std::string StoreItemListResult = "StoreItemListResult";
		const std::string CreateRoomResult = "CreateRoomResult";


		const std::string SUCCEED = "succeed";
		const std::string FAILURE = "failure";

		#define PRE_HANDLE(command) \
				Document document;\
				Pointer("/Command").Set(document, ##command);\
				StringBuffer buffer;\
				Writer<StringBuffer> writer(buffer);

		#define HANDLE_CATCH \
				catch (const std::exception & e)\
				{\
				printf("%s\n", e.what());\
				Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());\
				Pointer("/Paras/Info").Set(document, "·¢Éú´íÎó");\
				document.Accept(writer);\
				const char* output = buffer.GetString();\
				player->SendMessageFn(output);\
				}

	}
}
