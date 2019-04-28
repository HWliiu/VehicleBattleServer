#pragma once
#include <unordered_map>
#include "Singleton.h"
#include "RoomModel.h"

namespace GameServer
{
	using namespace Common;
	namespace Entity
	{
		class RoomManager :public Singleton<RoomManager>
		{
		public:
			friend class Singleton<RoomManager>;
			void AddRoom();
			void RemoveRoom();
			RoomModel* GetRoom(std::string roomId);
		private:
			RoomManager();
			~RoomManager();
			std::unordered_map<std::string, RoomModel> _roomMap;
		};
	}
}

