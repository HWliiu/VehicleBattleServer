#pragma once
#include <map>
#include <deque>
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
			std::string AddRoom(std::string roomName, std::string roomMode, std::string roomMap, PlayerModel* owner);
			void RemoveRoom(std::string roomId);
			void JoinRoom();
			void SearchRoom();
			void ChangeRoom();
			void ExitRoom();
			RoomModel* GetRoom(std::string roomId);
		private:
			RoomManager();
			~RoomManager();
			std::map<std::string, RoomModel> _roomMap;
			std::deque<std::string> _roomIdQueue;
			std::string PopRoomId();
			void PushRoomId(std::string roomId);
		};
	}
}

