#pragma once
#include "include/rapidjson/document.h"
#include <functional>

using namespace rapidjson;

namespace GameServer
{
	namespace Command
	{
		class LoginCommand
		{
		public:
			virtual void Execute(Document document, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage);
		};
		class RegisterCommand
		{
		public:
			virtual void Execute(Document document, std::function<void(std::string)> sendMessage);
		};


		class ICommand
		{
		public:
			virtual void Execute(Document document) = 0;
		};

		class LogoutCommand :public ICommand
		{
		public:
			virtual void Execute(Document document) override;
		};
		class ChangePasswordCommand :public ICommand
		{
		public:
			virtual void Execute(Document document) override;
		};

		class StoreItemListCommand :public ICommand
		{
		public:
			virtual void Execute(Document document) override;
		};
		class PurchaseCommand :public ICommand
		{
		public:
			virtual void Execute(Document document) override;
		};

		class ChangeVehicleCommand :public ICommand
		{
		public:
			virtual void Execute(Document document) override;
		};

		class CreateRoomCommand :public ICommand
		{
		public:
			virtual void Execute(Document document) override;
		};
		class RefreshRoomListCommand :public ICommand
		{
		public:
			virtual void Execute(Document document) override;
		};
		class JoinRoomCommand :public ICommand
		{
		public:
			virtual void Execute(Document document) override;
		};
		class SearchRoomCommand :public ICommand
		{
		public:
			virtual void Execute(Document document) override;
		};
	}
}

