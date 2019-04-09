#pragma once
#include <unordered_map>
#include "Command.h"

namespace GameServer
{
	using namespace Command;

	class CommandDispatcher
	{
	public:
		CommandDispatcher();
		~CommandDispatcher();

		void DispatchCommand(char* jsonData, std::function<void(std::string)> sendMessage);
	private:
		std::unordered_map<std::string, ICommand> _commandMap;
		void RegisterCommand();
	};
}

