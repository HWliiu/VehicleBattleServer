#pragma once
#include "PlayerModel.h"
#include "include/rapidjson/document.h"

namespace GameServer
{
	using namespace Entity;
	using namespace rapidjson;
	namespace Handle
	{
		class BattleHandle
		{
		public:
			void UpdateTransformState(std::string userId, Document document, PlayerModel* player);
			void UpdateFireState(std::string userId, Document document, PlayerModel* player);
			void UpdateHealthState(std::string userId, int health, PlayerModel* player);
			void EndGame(int rank, PlayerModel* player);
		};
	}
}

