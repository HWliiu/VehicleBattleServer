#pragma once
#include <list>
#include <string>
#include "PlayerModel.h"

namespace GameServer
{
	namespace Entity
	{
		class RoomModel
		{
		public:
			const std::string RoomId;
			const std::string RoomName;
			PlayerModel* Owner;
			std::string RoomMode;	//SingleMode,TeamMode (c++的class enum和string的互转换太麻烦了,直接用字符串表示)
			std::string RoomMap;	//Random,Map1,Map2,Map3
			bool IsGaming;

			list<PlayerModel*> PlayerList;

			RoomModel(std::string roomId, std::string roomName, std::string roomMode, std::string roomMap, PlayerModel* owner);
			~RoomModel();
			bool AddPlayer(PlayerModel* player);
			void RemovePlayer(PlayerModel* player);
			inline int GetPlayerNum() { return PlayerList.size(); }
		};
	}
}

