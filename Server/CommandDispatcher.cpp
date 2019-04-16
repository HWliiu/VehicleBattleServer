#include "pch.h"
#include "CommandDispatcher.h"
#include "include/rapidjson/pointer.h"

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
					///////////////////////////////////////////////////////
					std::cout << "recv:" << Util::U2G(message.c_str()) << std::endl;
					//lpPerHandleData->sendMessage(message);
					GetInstance()->DispatchCommand(message, lpPerHandleData->sendMessage);
				}
			}
		}
		void CommandDispatcher::DispatchCommand(std::string jsonData, std::function<void(std::string)> sendMessage)
		{
			Document document;
			document.Parse(jsonData.c_str());

			if (rapidjson::Value * commandValue = GetValueByPointer(document, "/Command"))
			{
				std::string command = commandValue->GetString();	//注意:这里执行了移动语义，commandValue所指向的Command在下一条语句被清空了
				EraseValueByPointer(document, "/Command");

				auto iter = _commandMap.find(command);
				if (iter != _commandMap.end())
				{
					_commandMap[iter->first]->Execute(std::move(document), sendMessage);	//注意:Document实现了移动语义
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
