#pragma once
#include <functional>
#include <string>
#include "PlayerModel.h"

namespace GameServer
{
	using namespace Entity;
	namespace Handle
	{
		class StoreHandle
		{
		public:
			void ListStoreItem(std::string userId, PlayerModel* player);
		};
	}
}

