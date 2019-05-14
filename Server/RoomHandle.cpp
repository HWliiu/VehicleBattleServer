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
					room->RemovePlayer(player);
					Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
					Pointer("/Paras/Info").Set(document, "�˳�����ɹ�");
					Pointer("/Paras/ExitPlayerId").Set(document, player->GetUserId().c_str());
					if (player == room->Owner)
					{
						if (room->PlayerList.size() > 0)	//����������ң�ת�÷���
						{
							room->Owner = room->PlayerList.front();
							Pointer("/Paras/NewOwnerId").Set(document, room->Owner->GetUserId().c_str());
							//֪ͨ�������
						}
						else	//û��������ң�ɾ������
						{
							RoomManager::GetInstance()->RemoveRoom(roomId);
							
							SERIALIZE_DOCUMENT;
							if (player->SendMessageFn != nullptr)
							{
								player->SendMessageFn(output);
							}
							return;
						}
					}

					SERIALIZE_DOCUMENT;
					if (player->SendMessageFn != nullptr)	//socket�Ͽ�ʱ��SendMessageFn��Ϊ��nullptr
					{
						player->SendMessageFn(output);
					}
					//֪ͨ�������
					for (auto& otherPlayer : room->PlayerList)
					{
						otherPlayer->SendMessageFn(output);
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "���䲻����");
					SERIALIZE_DOCUMENT;
					if (player->SendMessageFn != nullptr)
					{
						player->SendMessageFn(output);
					}
				}

			}
			HANDLE_CATCH(if (player->SendMessageFn != nullptr) player->SendMessageFn);
		}
	}
}
