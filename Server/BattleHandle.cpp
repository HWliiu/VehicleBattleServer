#include "pch.h"
#include "BattleHandle.h"
#include "include/rapidjson/pointer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/writer.h"
#include "Consts.h"
#include "RoomManager.h"
#include "DbUtil.h"


namespace GameServer
{
	namespace Handle
	{
		void BattleHandle::UpdateTransformState(std::string userId, Document document, PlayerModel* player)
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
		void BattleHandle::UpdateFireState(std::string userId, Document document, PlayerModel* player)
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
		void BattleHandle::UpdateHealthState(std::string userId, int health, PlayerModel* player)
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
					EndGame(room->GetPlayerNum() + 1, player);
				}
				if (room->GetPlayerNum() <= 1)
				{
					EndGame(1, room->PlayerList.front());
					RoomManager::GetInstance()->RemoveRoom(roomId);
				}
			}
			catch (...)
			{
			}
		}
		void BattleHandle::EndGame(int rank, PlayerModel* player)
		{
			CONSTRUCT_DOCUMENT(Common::EndGame.c_str());
			try
			{
				auto session = DBUtil::GetInstance()->GetSession();
				auto accountTable = session.getDefaultSchema().getTable("account");

				auto userExperience = (int)accountTable.select("experience").where("user_id=:userId").limit(1).bind("userId", player->GetUserId().c_str()).execute().fetchOne().get(0);
				auto userMoney = (int)accountTable.select("money").where("user_id=:userId").limit(1).bind("userId", player->GetUserId().c_str()).execute().fetchOne().get(0);

				auto experence = 8000 / rank;
				auto money = 8000 / rank;

				accountTable.update().set("money", userMoney + money).set("experience", userExperience + userMoney).where("user_id=:userId").limit(1).bind("userId", player->GetUserId().c_str()).execute();

				Pointer("/Paras/Rank").Set(document, rank);
				Pointer("/Paras/Experence").Set(document, experence);
				Pointer("/Paras/Money").Set(document, money);
				Pointer("/Paras/TotalExperence").Set(document, userExperience + userMoney);
				Pointer("/Paras/TotalMoney").Set(document, userMoney + money);
				SERIALIZE_DOCUMENT;
				player->SendMessageFn(output);
			}
			catch (const std::exception& e)
			{
				printf("%s\n", e.what());
			}
		}
	}
}
