#include "pch.h"
#include "BattleHandle.h"
#include "include/rapidjson/pointer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/writer.h"
#include "Consts.h"
#include "RoomManager.h"


namespace GameServer
{
	namespace Handle
	{
		void BattleHandle::UpdateTransformState(string userId, Document document, PlayerModel* player)
		{
			Pointer("/Command").Set(document, Common::UpdateTransformState.c_str());
			try
			{
				auto roomId = player->GetCurRoomId();
				auto roomManager = RoomManager::GetInstance();
				auto room = roomManager->GetRoom(roomId);

				if (room != nullptr)
				{
					SERIALIZE_DOCUMENT;
					for (auto& roomPlayer : room->PlayerList)
					{
						if (roomPlayer != player)
						{
							roomPlayer->SendMessageFn(output);
						}
					}
				}
			}
			catch (const std::exception&)
			{
				return;
			}
		}
	}
}
