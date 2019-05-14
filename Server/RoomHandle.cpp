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
					sprintf_s(info, "%s �˳�����", player->GetName().c_str());
					Pointer("/Paras/Info").Set(document, info);
					Pointer("/Paras/ExitPlayerId").Set(document, player->GetUserId().c_str());
					if (player == room->Owner)
					{
						if (room->PlayerList.size() > 0)	//����������ң�ת�÷���
						{
							room->Owner = room->PlayerList.front();
							sprintf_s(info, "%s �˳�����,%s ��Ϊ�˷���", player->GetName().c_str(), room->Owner->GetName().c_str());
							Pointer("/Paras/Info").Set(document, info);
							Pointer("/Paras/RoomInfo/OwnerId").Set(document, room->Owner->GetUserId().c_str());
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
					if (player->SendMessageFn != nullptr)	//socket�Ͽ�ʱ��SendMessageFn��Ϊ��nullptr
					{
						player->SendMessageFn(output);
					}
					//֪ͨ�������(player�Ѿ��ӷ���ɾ���ˣ����ﲻ�ᷢ��player)
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
					sprintf_s(info, "%s �ѱ������߳�����", kickPlayer->GetName().c_str());
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
					//���͸��������
					kickPlayer->SendMessageFn(output);
					//���͸������������
					for (auto& roomPlayer : room->PlayerList)
					{
						roomPlayer->SendMessageFn(output);
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "�����Ƿ��� ��Ȩ�߳����");
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
					Pointer("/Paras/Info").Set(document, "�����Ƿ��� �޷���ʼ��Ϸ");
					SERIALIZE_DOCUMENT;
					player->SendMessageFn(output);
					return;
				}

				if (room->GetPlayerNum() < 2)
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "���������������2�� �޷���ʼ��Ϸ");
					SERIALIZE_DOCUMENT;
					player->SendMessageFn(output);
					return;
				}

				for (auto& roomPlayer : room->PlayerList)
				{
					if (roomPlayer->GetPrepareState() == false)
					{
						Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
						Pointer("/Paras/Info").Set(document, "�������δ׼�� �޷���ʼ��Ϸ");
						SERIALIZE_DOCUMENT;
						player->SendMessageFn(output);
						return;
					}
				}

				Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
				Pointer("/Paras/Info").Set(document, "��ʼ��Ϸ");
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
