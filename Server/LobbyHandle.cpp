#include "pch.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/pointer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/writer.h"
#include "LobbyHandle.h"
#include "RoomManager.h"
#include "Consts.h"

namespace GameServer
{
	using namespace Common;
	using namespace rapidjson;
	namespace Handle
	{
		void LobbyHandle::CreateRoom(string userId, string roomName, string roomMode, string roomMap, PlayerModel* player)
		{
			PRE_HANDLE(Common::CreateRoomResult.c_str());

			try
			{
				auto roomManager = RoomManager::GetInstance();
				auto roomId = roomManager->AddRoom(roomName, roomMode, roomMap, player);
				auto room = roomManager->GetRoom(roomId);
				if (room != nullptr)
				{
					Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
					Pointer("/Paras/Info").Set(document, "�����ɹ�");
					//��ӷ�����Ϣ
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "����ʧ��");
				}
				document.Accept(writer);
				const char* output = buffer.GetString();
				player->SendMessageFn(output);
			}
			HANDLE_CATCH
		}
	}
}
