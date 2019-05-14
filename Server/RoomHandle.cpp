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
					char info[256];
					sprintf_s(info, "%s 退出房间", player->GetName().c_str());
					Pointer("/Paras/Info").Set(document, info);
					Pointer("/Paras/ExitPlayerId").Set(document, player->GetUserId().c_str());
					if (player == room->Owner)
					{
						if (room->PlayerList.size() > 0)	//还有其他玩家，转让房主
						{
							room->Owner = room->PlayerList.front();
							sprintf_s(info, "%s 退出房间,%s 成为了房主", player->GetName().c_str(), room->Owner->GetName().c_str());
							Pointer("/Paras/Info").Set(document, info);
							Pointer("/Paras/RoomInfo/OwnerId").Set(document, room->Owner->GetUserId().c_str());
						}
						else	//没有其他玩家，删除房间
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

					SERIALIZE_DOCUMENT;
					if (player->SendMessageFn != nullptr)	//socket断开时将SendMessageFn设为了nullptr
					{
						player->SendMessageFn(output);
					}
					//通知房间玩家(player已经从房间删除了，这里不会发给player)
					for (auto& otherPlayer : room->PlayerList)
					{
						otherPlayer->SendMessageFn(output);
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "房间不存在");
					SERIALIZE_DOCUMENT;
					if (player->SendMessageFn != nullptr)
					{
						player->SendMessageFn(output);
					}
				}

			}
			HANDLE_CATCH(if (player->SendMessageFn != nullptr) player->SendMessageFn);
		}
		void RoomHandle::ChangePrepareState(string userId, bool prepareState, PlayerModel * player)
		{
			CONSTRUCT_DOCUMENT(Common::ChangePrepareStateResult.c_str());

			try
			{
				auto roomId = player->GetCurRoomId();
				auto roomManager = RoomManager::GetInstance();
				auto room = roomManager->GetRoom(roomId);

				player->SetPrepareState(prepareState);

				Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
				Pointer("/Paras/Info").Set(document, "");
				Pointer("/Paras/PlayerId").Set(document, player->GetUserId().c_str());
				Pointer("/Paras/PrepareState").Set(document, player->GetPrepareState());

				SERIALIZE_DOCUMENT;
				for (auto& roomPlayer : room->PlayerList)
				{
					roomPlayer->SendMessageFn(output);
				}

			}
			HANDLE_CATCH(player->SendMessageFn);
		}
		void RoomHandle::KickPlayer(string userId, string playerId, PlayerModel * player)
		{
			CONSTRUCT_DOCUMENT(Common::KickPlayerResult.c_str());

			try
			{
				auto roomId = player->GetCurRoomId();
				auto roomManager = RoomManager::GetInstance();
				auto room = roomManager->GetRoom(roomId);

				if (userId == room->Owner->GetUserId())
				{
					auto kickPlayer = PlayerManager::GetInstance()->GetPlayer(playerId);
					room->RemovePlayer(kickPlayer);
					Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
					char info[256];
					sprintf_s(info, "%s 已被房主踢出房间", kickPlayer->GetName().c_str());
					Pointer("/Paras/Info").Set(document, info);
					Pointer("/Paras/KickPlayerId").Set(document, kickPlayer->GetUserId().c_str());

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
					SERIALIZE_DOCUMENT;
					//发送给被踢玩家
					kickPlayer->SendMessageFn(output);
					//发送给房间其他玩家
					for (auto& roomPlayer : room->PlayerList)
					{
						roomPlayer->SendMessageFn(output);
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "您不是房主 无权踢除玩家");
					SERIALIZE_DOCUMENT;
					player->SendMessageFn(output);
				}
			}
			HANDLE_CATCH(player->SendMessageFn);
		}
		void RoomHandle::SendMsg(string userId, string message, PlayerModel * player)
		{
			CONSTRUCT_DOCUMENT(Common::SendMessageResult.c_str());

			try
			{
				auto roomId = player->GetCurRoomId();
				auto roomManager = RoomManager::GetInstance();
				auto room = roomManager->GetRoom(roomId);

				Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
				Pointer("/Paras/Info").Set(document, "");
				Pointer("/Paras/PlayerName").Set(document, player->GetName().c_str());
				Pointer("/Paras/Message").Set(document, message.c_str());
				SERIALIZE_DOCUMENT;
				for (auto& roomPlayer : room->PlayerList)
				{
					roomPlayer->SendMessageFn(output);
				}
			}
			HANDLE_CATCH(player->SendMessageFn);
		}
		void RoomHandle::StartGame(string userId, PlayerModel * player)
		{
			CONSTRUCT_DOCUMENT(Common::StartGameResult.c_str());

			try
			{
				auto roomId = player->GetCurRoomId();
				auto roomManager = RoomManager::GetInstance();
				auto room = roomManager->GetRoom(roomId);

				if (player->GetUserId() != room->Owner->GetUserId())
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "您不是房主 无法开始游戏");
					SERIALIZE_DOCUMENT;
					player->SendMessageFn(output);
					return;
				}

				if (room->GetPlayerNum() < 2)
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "房间玩家人数低于2人 无法开始游戏");
					SERIALIZE_DOCUMENT;
					player->SendMessageFn(output);
					return;
				}

				for (auto& roomPlayer : room->PlayerList)
				{
					if (roomPlayer->GetPrepareState() == false)
					{
						Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
						Pointer("/Paras/Info").Set(document, "还有玩家未准备 无法开始游戏");
						SERIALIZE_DOCUMENT;
						player->SendMessageFn(output);
						return;
					}
				}

				Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
				Pointer("/Paras/Info").Set(document, "开始游戏");
				room->IsGaming = true;
				SERIALIZE_DOCUMENT;
				for (auto& roomPlayer : room->PlayerList)
				{
					roomPlayer->SendMessageFn(output);
				}
			}
			HANDLE_CATCH(player->SendMessageFn);
		}
	}
}
