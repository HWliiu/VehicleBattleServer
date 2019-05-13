#include "pch.h"
#include "CommandDispatcher.h"
#include "include/rapidjson/pointer.h"
#include "SmallTools.h"
#include "PlayerManager.h"
#include "Consts.h"

namespace GameServer
{
	namespace Service
	{
		CommandDispatcher::CommandDispatcher()
		{
			InitCommandMap();
		}

		CommandDispatcher::~CommandDispatcher()
		{
			for (auto iter = _commandMap.begin(); iter != _commandMap.end(); iter++)
			{
				delete iter->second;
				iter->second = nullptr;
				_commandMap.erase(iter);
			}
		}
		void CommandDispatcher::StartDispatch(PerHandleData* lpPerHandleData)
		{
			while (true)
			{
				std::string message = lpPerHandleData->recvMsgQueue.Dequeue();
				if (message.empty())
				{
					break;
				}
				else
				{
					/////////////////////////////////////////////////////////
					std::cout << "recv:" << U2G(message.c_str()) << std::endl;
					GetInstance()->DispatchCommand(message, lpPerHandleData->socket, lpPerHandleData->sendMessageFn);
				}
			}
		}
		void CommandDispatcher::NotifyDisconnect(unsigned __int64 connSocket)
		{
			auto player = Entity::PlayerManager::GetInstance()->GetPlayerBySocket(connSocket);
			if (player != nullptr)
			{
				//退出房间
				if (!player->GetCurRoomId().empty)
				{
					Document document;
					Pointer("Command").Set(document, Common::RequestExitRoom.c_str());
					Pointer("/Paras/UserId").Set(document, player->GetUserId().c_str());
					Pointer("/Paras/Token").Set(document, player->GetToken().c_str());
					_commandMap[Common::RequestExitRoom]->Execute(std::move(document));
				}
				Entity::PlayerManager::GetInstance()->RemovePlayer(player->GetUserId());
			}
		}
		void CommandDispatcher::DispatchCommand(std::string jsonData, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
			Document document;
			try
			{
				document.Parse(jsonData.c_str());
			}
			catch (const std::exception&)
			{
				printf("document parse error\n");
				return;
			}

			if (rapidjson::Value * commandValue = GetValueByPointer(document, "/Command"))
			{
				std::string command = commandValue->GetString();
				EraseValueByPointer(document, "/Command");

				//判断是否为登录注册命令(这两个命令参数不一样，单独分出来)
				if (command == Common::RequestLogin)
				{
					_loginCommand.Execute(std::move(document), connSocket, sendMessage);
					return;
				}
				if (command == Common::RequestRegister)
				{
					_registerCommand.Execute(std::move(document), sendMessage);
					return;
				}

				auto iter = _commandMap.find(command);
				if (iter != _commandMap.end())
				{
					_commandMap[iter->first]->Execute(std::move(document));
				}
				else
				{
					printf("command not found\n");
				}
			}
			else
			{
				printf("message format error\n");
			}

		}
		void CommandDispatcher::InitCommandMap()
		{
			//所有命令到这里注册(键要跟客户端定义的命令常量匹配)
			_commandMap[Common::RequestLogout] = new LogoutCommand();
			_commandMap[Common::RequestChangePassword] = new ChangePasswordCommand();
			_commandMap[Common::RequestStoreItemList] = new StoreItemListCommand();
			_commandMap[Common::RequestPurchaseItem] = new PurchaseCommand();
			_commandMap[Common::RequestChangeVehicle] = new ChangeVehicleCommand();
			_commandMap[Common::RequestCreateRoom] = new CreateRoomCommand();
			_commandMap[Common::RequestRefreshRoomList] = new RefreshRoomListCommand();
			_commandMap[Common::RequestJoinRoom] = new JoinRoomCommand();
			_commandMap[Common::RequestSearchRoom] = new SearchRoomCommand();
			_commandMap[Common::RequestExitRoom] = new ExitRoomCommand();
		}
	}
}
