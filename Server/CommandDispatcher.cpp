#include "pch.h"
#include "CommandDispatcher.h"
#include "include/rapidjson/pointer.h"
#include "SmallTools.h"
#include "PlayerManager.h"

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

				auto iter = _commandMap.find(command);
				if (iter != _commandMap.end())
				{
					_commandMap[iter->first]->Execute(std::move(document), connSocket, sendMessage);
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
			_commandMap["RequestLogin"] = new LoginCommand();
			_commandMap["RequestRegister"] = new RegisterCommand();
		}
	}
}
