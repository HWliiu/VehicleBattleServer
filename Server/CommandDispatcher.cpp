#include "pch.h"
#include "CommandDispatcher.h"

namespace GameServer
{
	CommandDispatcher::CommandDispatcher()
	{
		InitCommandMap();
	}


	CommandDispatcher::~CommandDispatcher()
	{
	}
	void CommandDispatcher::DispatchCommand(char* jsonData, std::function<void(std::string)> sendMessage)
	{
		Document document;
		document.Parse(jsonData);

		if (Value * commandValue = GetValueByPointer(document, "/Command"))
		{
			std::string command = commandValue->GetString();	//ע��:����ִ�����ƶ����壬commandValue��ָ���Command����һ����䱻�����
			EraseValueByPointer(document, "/Command");

			auto iter = _commandMap.find(command);
			if (iter != _commandMap.end())
			{
				_commandMap[iter->first]->Execute(std::move(document), sendMessage);	//ע��:Documentʵ�����ƶ�����
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
		//�����������ע��(��ֵҪ���ͻ��˶���������ƥ��)
		_commandMap["RequestLogin"] = new LoginCommand();
		_commandMap["RequestRegister"] = new RegisterCommand();
	}
}
