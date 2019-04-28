#pragma once
#include <unordered_map>
#include "Command.h"
#include "IOCPServer.h"
#include "Singleton.h"

namespace GameServer
{
	using namespace Command;
	using namespace Common;
	namespace Service
	{
		class CommandDispatcher :public Singleton<CommandDispatcher>
		{
		public:
			friend class Singleton<CommandDispatcher>;
			static void StartDispatch(PerHandleData* lpPerHandleData);
			void NotifyDisconnect(unsigned __int64 connSocket);
		private:
			LoginCommand _loginCommand;
			RegisterCommand _registerCommand;
			std::unordered_map<std::string, ICommand*> _commandMap;

			CommandDispatcher();
			~CommandDispatcher();
			void InitCommandMap();
			void DispatchCommand(std::string jsonData, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage);
		};
	}
}

