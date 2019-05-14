#include "pch.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/pointer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/writer.h"
#include "LobbyHandle.h"
#include "RoomManager.h"
#include "PlayerManager.h"
#include "Consts.h"

namespace GameServer
{
	using namespace Common;
	using namespace rapidjson;
	namespace Handle
	{
		void LobbyHandle::CreateRoom(string userId, string roomName, string roomMode, string roomMap, PlayerModel* player)
		{
			CONSTRUCT_DOCUMENT(Common::CreateRoomResult.c_str());

			try
			{
				//检查参数是否正确
				if (roomMode != "SingleMode" && roomMode != "TeamMode")
				{
					throw std::exception("roomMode error");
				}
				if (roomMap != "Random" && roomMap != "Map1" && roomMap != "Map2" && roomMap != "Map3")
				{
					throw std::exception("roomMap error");
				}

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
				SERIALIZE_DOCUMENT;
				player->SendMessageFn(output);
			}
			HANDLE_CATCH(player->SendMessageFn);
		}
		void LobbyHandle::RefreshRoomList(string userId, PlayerModel* player)
		{
			CONSTRUCT_DOCUMENT(Common::RefreshRoomListResult.c_str());

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

				SERIALIZE_DOCUMENT;
				player->SendMessageFn(output);
			}
			HANDLE_CATCH(player->SendMessageFn);
		}
		void LobbyHandle::JoinRoom(string userId, string roomId, PlayerModel* player)
		{
			CONSTRUCT_DOCUMENT(Common::JoinRoomResult.c_str());

			try
			{
				auto roomManager = RoomManager::GetInstance();
				auto room = roomManager->GetRoom(roomId);
				if (room != nullptr)
				{
					if (room->IsGaming)
					{
						Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
						Pointer("/Paras/Info").Set(document, "该房间已开始游戏");
						SERIALIZE_DOCUMENT;
						player->SendMessageFn(output);
						return;
					}
					if (room->AddPlayer(player))
					{
						Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
						Pointer("/Paras/Info").Set(document, "加入成功");

						Pointer("/Paras/RoomInfo/RoomId").Set(document, room->RoomId.c_str());
						Pointer("/Paras/RoomInfo/RoomName").Set(document, room->RoomName.c_str());
						Pointer("/Paras/RoomInfo/OwnerId").Set(document, room->Owner->GetUserId().c_str());
						Pointer("/Paras/RoomInfo/OwnerName").Set(document, room->Owner->GetName().c_str());
						Pointer("/Paras/RoomInfo/RoomMode").Set(document, room->RoomMode.c_str());
						Pointer("/Paras/RoomInfo/RoomMap").Set(document, room->RoomMap.c_str());
						Pointer("/Paras/RoomInfo/PlayerNum").Set(document, room->PlayerList.size());

						char path[256];
						int i = 0;
						for (auto& player : room->PlayerList)
						{
							sprintf_s(path, "/Paras/RoomInfo/PlayerList/%d/PlayerId", i);
							Pointer(path).Set(document, player->GetUserId().c_str());
							sprintf_s(path, "/Paras/RoomInfo/PlayerList/%d/PlayerName", i);
							Pointer(path).Set(document, player->GetName().c_str());
							sprintf_s(path, "/Paras/RoomInfo/PlayerList/%d/PlayerLevel", i);
							Pointer(path).Set(document, player->GetLevel().c_str());
							sprintf_s(path, "/Paras/RoomInfo/PlayerList/%d/PrepareState", i);
							Pointer(path).Set(document, player->GetPrepareState());
							/*sprintf_s(path, "/Paras/RoomInfo/PlayerList/%d/PlayerTeam", i);
							Pointer(path).Set(document, player->GetTeam().c_str());*/
							sprintf_s(path, "/Paras/RoomInfo/PlayerList/%d/VehicleInfo/Id", i);
							Pointer(path).Set(document, player->GetCurVehicle()->VehicleId.c_str());
							sprintf_s(path, "/Paras/RoomInfo/PlayerList/%d/VehicleInfo/Name", i);
							Pointer(path).Set(document, player->GetCurVehicle()->VehicleName.c_str());
							sprintf_s(path, "/Paras/RoomInfo/PlayerList/%d/VehicleInfo/Type", i);
							Pointer(path).Set(document, player->GetCurVehicle()->VehicleType.c_str());
							sprintf_s(path, "/Paras/RoomInfo/PlayerList/%d/VehicleInfo/Attack", i);
							Pointer(path).Set(document, player->GetCurVehicle()->Attack);
							sprintf_s(path, "/Paras/RoomInfo/PlayerList/%d/VehicleInfo/Defend", i);
							Pointer(path).Set(document, player->GetCurVehicle()->Defend);
							sprintf_s(path, "/Paras/RoomInfo/PlayerList/%d/VehicleInfo/Motility", i);
							Pointer(path).Set(document, player->GetCurVehicle()->Motility);
							sprintf_s(path, "/Paras/RoomInfo/PlayerList/%d/VehicleInfo/MaxHealth", i);
							Pointer(path).Set(document, player->GetCurVehicle()->MaxHealth);

							i++;
						}
						//通知其他玩家
						NotifyOtherPlayersJoin(room, player);
					}
					else
					{
						Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
						Pointer("/Paras/Info").Set(document, "该房间人数已满");
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "房间未找到");
				}

				SERIALIZE_DOCUMENT;
				player->SendMessageFn(output);
			}
			HANDLE_CATCH(player->SendMessageFn);
		}
		void LobbyHandle::NotifyOtherPlayersJoin(RoomModel* room, PlayerModel* joinPlayer)
		{
			CONSTRUCT_DOCUMENT(Common::NetPlayerJoinRoom.c_str());

			Pointer("/Paras/PlayerInfo/PlayerId").Set(document, joinPlayer->GetUserId().c_str());
			Pointer("/Paras/PlayerInfo/PlayerName").Set(document, joinPlayer->GetName().c_str());
			Pointer("/Paras/PlayerInfo/PlayerLevel").Set(document, joinPlayer->GetLevel().c_str());
			Pointer("/Paras/PlayerInfo/PrepareState").Set(document, joinPlayer->GetPrepareState());
			Pointer("/Paras/PlayerInfo/VehicleInfo/Id").Set(document, joinPlayer->GetCurVehicle()->VehicleId.c_str());
			Pointer("/Paras/PlayerInfo/VehicleInfo/Name").Set(document, joinPlayer->GetCurVehicle()->VehicleName.c_str());
			Pointer("/Paras/PlayerInfo/VehicleInfo/Type").Set(document, joinPlayer->GetCurVehicle()->VehicleType.c_str());
			Pointer("/Paras/PlayerInfo/VehicleInfo/Attack").Set(document, joinPlayer->GetCurVehicle()->Attack);
			Pointer("/Paras/PlayerInfo/VehicleInfo/Defend").Set(document, joinPlayer->GetCurVehicle()->Defend);
			Pointer("/Paras/PlayerInfo/VehicleInfo/Motility").Set(document, joinPlayer->GetCurVehicle()->Motility);
			Pointer("/Paras/PlayerInfo/VehicleInfo/MaxHealth").Set(document, joinPlayer->GetCurVehicle()->MaxHealth);

			SERIALIZE_DOCUMENT;

			for (auto& player : room->PlayerList)
			{
				if (joinPlayer != player)
				{
					player->SendMessageFn(output);
				}
			}
		}

		void LobbyHandle::SearchRoom(string userId, string roomId, PlayerModel* player)
		{
			CONSTRUCT_DOCUMENT(Common::SearchRoomResult.c_str());

			auto roomManager = RoomManager::GetInstance();
			auto room = roomManager->GetRoom(roomId);
			if (room != nullptr)
			{
				Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
				Pointer("/Paras/Info").Set(document, "查找成功");

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
				Pointer("/Paras/Info").Set(document, "房间未找到");
			}
			SERIALIZE_DOCUMENT;
			player->SendMessageFn(output);
		}
	}
}
