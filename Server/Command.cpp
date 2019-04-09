#include "pch.h"
#include "Command.h"

namespace GameServer
{
	namespace Command
	{
		void LoginCommand::Execute(Document document, std::function<void(std::string)> sendMessage)
		{
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			document.Accept(writer);
			const char* output = buffer.GetString();
			printf("%s\n", output);
			sendMessage(output);
		}
		void RegisterCommand::Execute(Document document, std::function<void(std::string)> sendMessage)
		{
		}
	}
}

