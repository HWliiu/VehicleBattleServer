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
		void BattleHandle::UpdateFireState(string userId, Document document, PlayerModel* player)
		{
			Pointer("/Command").Set(document, Common::UpdateFireState.c_str());
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
		void BattleHandle::UpdateHealthState(string userId, int health, PlayerModel* player)
		{
			CONSTRUCT_DOCUMENT(Common::UpdateHealthState.c_str());

			try
			{
				auto roomId = player->GetCurRoomId();
				auto roomManager = RoomManager::GetInstance();
				auto room = roomManager->GetRoom(roomId);

				player->SetHealth(health);

				Pointer("/Paras/PlayerId").Set(document, player->GetUserId().c_str());
				Pointer("/Paras/Health").Set(document, player->GetHealth());

				SERIALIZE_DOCUMENT;
				for (auto& roomPlayer : room->PlayerList)
				{
					if (roomPlayer->GetUserId() != player->GetUserId())
					{
						roomPlayer->SendMessageFn(output);
					}
				}

				if (player->GetHealth() < 0)
				{
					room->RemovePlayer(player);
				}
			}
			catch (...)
			{
			}
		}
	}
}
