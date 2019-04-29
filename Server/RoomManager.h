#pragma once
#include <unordered_map>
#include <vector>
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
			void CreateRoom();
			void RemoveRoom();
			void JoinRoom();
			void SearchRoom();
			void ChangeRoom();
			void ExitRoom();


			RoomModel* GetRoom(std::string roomId);
		private:
			RoomManager();
			~RoomManager();
			std::unordered_map<std::string, RoomModel> _roomMap;
		};
	}
}

