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
					Pointer("/Paras/Info").Set(document, "创建成功");
					Pointer("/Paras/RoomInfo/RoomId").Set(document, room->RoomId.c_str());
					Pointer("/Paras/RoomInfo/RoomName").Set(document, room->RoomName.c_str());
					Pointer("/Paras/RoomInfo/OwnerId").Set(document, room->Owner->GetUserId().c_str());
					Pointer("/Paras/RoomInfo/OwnerName").Set(document, room->Owner->GetName().c_str());
					Pointer("/Paras/RoomInfo/RoomMode").Set(document, room->RoomMode.c_str());
					Pointer("/Paras/RoomInfo/RoomMap").Set(document, room->RoomMap.c_str());
					Pointer("/Paras/RoomInfo/PlayerNum").Set(document, room->PlayerList.size());
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "创建失败");
				}
				SENDMESSAGE2;
			}
			HANDLE_CATCH2
		}
		void LobbyHandle::RefreshRoomList(string userId, PlayerModel* player)
		{
			PRE_HANDLE(Common::RefreshRoomListResult.c_str());

			try
			{
				Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
				Pointer("/Paras/Info").Set(document, "刷新成功");
				auto roomManager = RoomManager::GetInstance();
				auto&& roomList = roomManager->GetAllRoom();

				char path[64];
				int i = 0;
				for (auto& room : roomList)
				{
					sprintf_s(path, "/Paras/RoomList/%d/RoomInfo/RoomId", i);
					Pointer(path).Set(document, room->RoomId.c_str());
					sprintf_s(path, "/Paras/RoomList/%d/RoomInfo/RoomName", i);
					Pointer(path).Set(document, room->RoomName.c_str());
					sprintf_s(path, "/Paras/RoomList/%d/RoomInfo/OwnerId", i);
					Pointer(path).Set(document, room->Owner->GetUserId().c_str());
					sprintf_s(path, "/Paras/RoomList/%d/RoomInfo/OwnerName", i);
					Pointer(path).Set(document, room->Owner->GetName().c_str());
					sprintf_s(path, "/Paras/RoomList/%d/RoomInfo/RoomMode", i);
					Pointer(path).Set(document, room->RoomMode.c_str());
					sprintf_s(path, "/Paras/RoomList/%d/RoomInfo/RoomMap", i);
					Pointer(path).Set(document, room->RoomMap.c_str());
					sprintf_s(path, "/Paras/RoomList/%d/RoomInfo/PlayerNum", i);
					Pointer(path).Set(document, room->PlayerList.size());

					i++;
				}

				SENDMESSAGE2;
			}
			HANDLE_CATCH2
		}
	}
}
