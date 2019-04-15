#pragma once
#include <unordered_map>
#include "Command.h"

namespace GameServer
{
	using namespace Command;
	namespace Service
	{
		class CommandDispatcher
		{
		public:
			CommandDispatcher();
			~CommandDispatcher();

			void DispatchCommand(std::string jsonData, std::function<void(std::string)> sendMessage);
		private:
			std::unordered_map<std::string, ICommand*> _commandMap;
			void InitCommandMap();
		};
	}
}

