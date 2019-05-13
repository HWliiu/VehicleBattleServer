#include "pch.h"
#include "RoomHandle.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/pointer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/writer.h"
#include "RoomManager.h"
#include "PlayerManager.h"
#include "Consts.h"

namespace GameServer
{
	using namespace Entity;
	using namespace rapidjson;
	namespace Handle
	{
		void RoomHandle::ExitRoom(string userId, PlayerModel* player)
		{
			CONSTRUCT_DOCUMENT(Common::ExitRoomResult.c_str());

			try
			{
				auto roomId = player->GetCurRoomId();
				auto roomManager = RoomManager::GetInstance();
				auto room = roomManager->GetRoom(roomId);

				if (room != nullptr)
				{
					if (player == room->Owner)
					{
						if (room->PlayerList.size() > 1)	//还有其他玩家，转让房主
						{
							room->RemovePlayer(player);
							Owner = PlayerList.front();
							//通知其他玩家
						}
						else	//没有其他玩家，删除房间
						{
							room->RemovePlayer(player);
							RoomManager::GetInstance()->RemoveRoom(roomId);
							Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
							Pointer("/Paras/Info").Set(document, "退出房间成功");
							Pointer("/Paras/PlayerId").Set(document, player->GetUserId().c_str());
							SERIALIZE_DOCUMENT;
							player->SendMessageFn(output);
						}
					}
					else
					{
						room->RemovePlayer(player);
						Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
						Pointer("/Paras/Info").Set(document, "退出房间成功");
						Pointer("/Paras/PlayerId").Set(document, player->GetUserId().c_str());
						SERIALIZE_DOCUMENT;
						player->SendMessageFn(output);
						//通知其他玩家
						for (auto& otherPlayer : room->PlayerList)
						{
							otherPlayer->SendMessageFn(output);
						}
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "房间不存在");
					SERIALIZE_DOCUMENT;
					player->SendMessageFn(output);
				}

			}
			HANDLE_CATCH(player->SendMessageFn);
		}
	}
}
