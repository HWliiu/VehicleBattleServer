#include "pch.h"
#include "CommandDispatch.h"

namespace GameServer
{
	CommandDispatcher::CommandDispatcher()
	{
		RegisterCommand();
	}


	CommandDispatcher::~CommandDispatcher()
	{
	}
	void CommandDispatcher::DispatchCommand(char* jsonData, std::function<void(std::string)> sendMessage)
	{
		Document d;
		d.Parse(jsonData);
		_commandMap[""].Execute(d, sendMessage);
	}
	void CommandDispatcher::RegisterCommand()
	{
		//所有命令到这里注册
	}
}
