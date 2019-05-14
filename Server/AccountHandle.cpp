#include "pch.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/pointer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/writer.h"
#include "AccountHandle.h"
#include "DbUtil.h"
#include "SmallTools.h"
#include "PlayerManager.h"
#include "Consts.h"

namespace GameServer
{
	using namespace Common;
	using namespace rapidjson;
	namespace Handle
	{
		void AccountHandle::Login(std::string username, std::string password, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
			CONSTRUCT_DOCUMENT(Common::LoginResult.c_str());

			try
			{
				auto session = DBUtil::GetInstance()->GetSession();
				auto accountTable = session.getDefaultSchema().getTable("account");

				auto accountRowResult = accountTable.select().where("username=:username").limit(1).bind("username", username).execute();
				if (accountRowResult.count() > 0)
				{
					//��ȡ�û���Ϣ
					auto accountRow = accountRowResult.fetchOne();

					auto id = to_string((int)accountRow[0]);
					auto username = (std::string)accountRow[1];
					auto pwd = (std::string)accountRow[2];
					auto experience = to_string((int)accountRow[3]);
					auto money = to_string((int)accountRow[4]);
					auto level = to_string((int)accountRow[5]);
					auto curVehicleId = to_string((int)accountRow[6]);
					auto registerTime = accountRow[7].isNull() ? "" : ((std::string)accountRow[7]);
					auto lastLoginTime = accountRow[8].isNull() ? "" : ((std::string)accountRow[8]);
					//��֤����
					if (pwd == password)
					{
						//�鿴�û��Ƿ��ѵ�¼
						if (PlayerManager::GetInstance()->GetPlayer(id) != nullptr)
						{
							auto player = PlayerManager::GetInstance()->GetPlayer(id);
							if (connSocket != player->ConnSocket)	//���豸��¼
							{
								//���߲����µ�¼
								Logout(id, player, false);
								//PlayerManager::GetInstance()->RemovePlayer(id);
								closesocket(player->ConnSocket);	//ֻ�������Լ��ͻ����������ʱ�����������ر��׽���
							}
							else
							{
								Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
								Pointer("/Paras/Info").Set(document, "���ѵ�¼ �����ظ���¼");
								SERIALIZE_DOCUMENT;
								sendMessage(output);
								return;
							}
						}
						std::string token = GenerateGuid();	//����guid���������֤
						Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
						Pointer("/Paras/Info").Set(document, "��¼�ɹ�");
						Pointer("/Paras/UserInfo/Id").Set(document, id.c_str());
						Pointer("/Paras/UserInfo/UserName").Set(document, username.c_str());
						Pointer("/Paras/UserInfo/Token").Set(document, token.c_str());
						Pointer("/Paras/UserInfo/Experience").Set(document, experience.c_str());
						Pointer("/Paras/UserInfo/Money").Set(document, money.c_str());
						Pointer("/Paras/UserInfo/Level").Set(document, level.c_str());
						Pointer("/Paras/UserInfo/CurVehicleId").Set(document, curVehicleId.c_str());
						Pointer("/Paras/UserInfo/RegisterTime").Set(document, registerTime.c_str());
						Pointer("/Paras/UserInfo/LastLoginTime").Set(document, lastLoginTime.c_str());

						//��ȡ�ؾ���Ϣ
						auto vehicleResult = session.sql("SELECT "
							"vehicle.vehicle_id, "
							"vehicle.vehicle_name, "
							"vehicle.vehicle_type, "
							"vehicle.vehicle_attack, "
							"vehicle.vehicle_motility, "
							"vehicle.vehicle_defend, "
							"vehicle.vehicle_health, "
							"vehicle.vehicle_price, "
							"vehicle.vehicle_intro "
							"FROM "
							"possess_vehicle "
							"INNER JOIN vehicle ON possess_vehicle.vehicle_id = vehicle.vehicle_id "
							"WHERE "
							"possess_vehicle.user_id = ?").bind(id).execute();

						int count = vehicleResult.count();
						vector<VehicleModel> vehicleList;
						for (size_t i = 0; i < count; i++)
						{
							Row vehicleRow = vehicleResult.fetchOne();
							char path[64];

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Id", i);
							auto vehicleId = to_string((int)vehicleRow[0]);
							Pointer(path).Set(document, vehicleId.c_str());

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Name", i);
							auto vehicleName = MysqlxBytes2String(vehicleRow.getBytes(1));	//�����ַ���ֱ�ӻ�ȡ�����쳣
							Pointer(path).Set(document, vehicleName.c_str());

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Type", i);
							auto vehicleType = (std::string)vehicleRow[2];
							Pointer(path).Set(document, vehicleType.c_str());

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Attack", i);
							auto vehicleAttack = (float)vehicleRow[3];
							Pointer(path).Set(document, vehicleAttack);

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Motility", i);
							auto vehicleMotility = (float)vehicleRow[4];
							Pointer(path).Set(document, vehicleMotility);

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Defend", i);
							auto vehicleDefend = (float)vehicleRow[5];
							Pointer(path).Set(document, vehicleDefend);

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/MaxHealth", i);
							auto vehicleMaxHealth = (float)vehicleRow[6];
							Pointer(path).Set(document, vehicleMaxHealth);

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Price", i);
							auto vehiclePrice = (int)(float)vehicleRow[7];
							Pointer(path).Set(document, vehiclePrice);

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Intro", i);
							auto vehicleIntro = MysqlxBytes2String(vehicleRow.getBytes(8));
							Pointer(path).Set(document, vehicleIntro.c_str());

							vehicleList.emplace_back(vehicleId, vehicleName, vehicleType, vehicleAttack, vehicleMotility, vehicleDefend, vehicleMaxHealth, vehiclePrice, vehicleIntro);
						}
						//����û�
						PlayerManager::GetInstance()->AddPlayer(id, username, token, experience, money, level, curVehicleId, registerTime, lastLoginTime, vehicleList, connSocket, sendMessage);
						//���µ�¼ʱ��
						accountTable.update().set("last_login_time", GetCurTime()).where("username=:username").limit(1).bind("username", username).execute();

						SERIALIZE_DOCUMENT;
						sendMessage(output);
					}
					else
					{
						Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
						Pointer("/Paras/Info").Set(document, "�������");
						SERIALIZE_DOCUMENT;
						sendMessage(output);
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "���û�������");
					SERIALIZE_DOCUMENT;
					sendMessage(output);
				}
			}
			HANDLE_CATCH(sendMessage);
		}

		void AccountHandle::Register(std::string username, std::string password, std::function<void(std::string)> sendMessage)
		{
			CONSTRUCT_DOCUMENT(Common::RegisterResult.c_str());

			try
			{
				auto session = DBUtil::GetInstance()->GetSession();
				auto accountTable = session.getDefaultSchema().getTable("account");

				//�����û����Ƿ��Ѵ���
				auto userRowResult = accountTable.select("username").where("username=:username").limit(1).bind("username", username).execute();
				if (userRowResult.count() > 0)
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "���û����Ѵ���");
					SERIALIZE_DOCUMENT;
					sendMessage(output);
					return;
				}
				//����û�
				session.startTransaction();
				auto insertResult = accountTable.insert("username", "password", "register_time").values(username, password, GetCurTime()).execute();
				if (insertResult.getAffectedItemsCount() == 1)
				{
					//��ӳ�ʼ��Ϣ
					int userId = accountTable.select("user_id").where("username=:username").limit(1).bind("username", username).execute().fetchOne()[0];
					auto possessVehicle = session.getDefaultSchema().getTable("possess_vehicle");
					possessVehicle.insert("user_id", "vehicle_id").values(userId, 1).execute();	//Ĭ��ӵ��idΪ1���ؾ�
					session.commit();

					Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
					Pointer("/Paras/Info").Set(document, "ע��ɹ�");
					SERIALIZE_DOCUMENT;
					sendMessage(output);
				}
				else
				{
					session.rollback();
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "ע��ʧ��");
					SERIALIZE_DOCUMENT;
					sendMessage(output);
				}
			}
			HANDLE_CATCH(sendMessage);
		}

		void AccountHandle::Logout(string userId, PlayerModel* player, bool normalLogout)
		{
			CONSTRUCT_DOCUMENT(Common::LogoutResult.c_str());

			auto sendMessage = player->SendMessageFn;
			if (normalLogout)
			{
				PlayerManager::GetInstance()->RemovePlayer(userId);
				Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
				Pointer("/Paras/Info").Set(document, "�ɹ��˳���¼");
				Pointer("/Paras/NormalLogout").Set(document, normalLogout);
			}
			else
			{
				PlayerManager::GetInstance()->RemovePlayer(userId);
				Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
				Pointer("/Paras/Info").Set(document, "�����˺��������豸��¼");
				Pointer("/Paras/NormalLogout").Set(document, normalLogout);
			}
			SERIALIZE_DOCUMENT;
			sendMessage(output);
		}

		void AccountHandle::ChangePassword(string userId, string oldPassword, string newPassword, PlayerModel* player)
		{
			CONSTRUCT_DOCUMENT(Common::ChangePasswordResult.c_str());

			auto sendMessage = player->SendMessageFn;
			try
			{
				auto session = DBUtil::GetInstance()->GetSession();
				auto accountTable = session.getDefaultSchema().getTable("account");

				//��֤����
				auto accountRowResult = accountTable.select("password").where("user_id=:userId").limit(1).bind("userId", userId).execute();
				if ((std::string)(accountRowResult.fetchOne().get(0)) == oldPassword)
				{
					auto result = accountTable.update().set("password", newPassword).where("user_id=:userId").limit(1).bind("userId", userId).execute();
					if (result.getAffectedItemsCount() == 1)
					{
						Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
						Pointer("/Paras/Info").Set(document, "�޸ĳɹ�");
						//�˳���¼
						/*Logout();*/
						PlayerManager::GetInstance()->RemovePlayer(userId);
					}
					else
					{
						Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
						Pointer("/Paras/Info").Set(document, "�޸�ʧ��");
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "ԭ���벻��ȷ");
				}
				SERIALIZE_DOCUMENT;
				sendMessage(output);
			}
			HANDLE_CATCH(sendMessage);
		}
		void AccountHandle::PurchaseVehicle(string userId, string vehicleId, PlayerModel* player)
		{
			CONSTRUCT_DOCUMENT(Common::PurchaseItemResult.c_str());

			try
			{
				auto session = DBUtil::GetInstance()->GetSession();
				auto accountTable = session.getDefaultSchema().getTable("account");
				auto vehicleTable = session.getDefaultSchema().getTable("vehicle");
				auto possessVehicleTable = session.getDefaultSchema().getTable("possess_vehicle");

				if (possessVehicleTable.select().where("user_id=:userId and vehicle_id=:vehicleId").bind("userId", userId).bind("vehicleId", vehicleId).execute().count() > 0)
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "����ӵ�д���Ʒ");
					SERIALIZE_DOCUMENT;
					player->SendMessageFn(output);
					return;
				}

				auto userMoney = (int)accountTable.select("money").where("user_id=:userId").limit(1).bind("userId", userId).execute().fetchOne().get(0);
				auto vehiclePrice = (int)(float)vehicleTable.select("vehicle_price").where("vehicle_id=:vehicleId").limit(1).bind("vehicleId", vehicleId).execute().fetchOne().get(0);

				if (userMoney >= vehiclePrice)
				{
					try
					{
						session.startTransaction();
						accountTable.update().set("money", userMoney - vehiclePrice).where("user_id=:userId").limit(1).bind("userId", userId).execute();
						possessVehicleTable.insert("user_id", "vehicle_id").values(userId, vehicleId).execute();

						Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
						Pointer("/Paras/Info").Set(document, "����ɹ�");
						Pointer("/Paras/UserInfo/Money").Set(document, to_string(userMoney - vehiclePrice).c_str());

						Row vehicleRow = vehicleTable.select().where("vehicle_id=:vehicleId").limit(1).bind("vehicleId", vehicleId).execute().fetchOne();
						auto vehicleId = to_string((int)vehicleRow[0]);
						Pointer("/Paras/VehicleInfo/Id").Set(document, vehicleId.c_str());
						auto vehicleName = MysqlxBytes2String(vehicleRow.getBytes(1));
						Pointer("/Paras/VehicleInfo/Name").Set(document, vehicleName.c_str());
						auto vehicleType = (std::string)vehicleRow[2];
						Pointer("/Paras/VehicleInfo/Type").Set(document, vehicleType.c_str());
						auto vehicleAttack = (float)vehicleRow[3];
						Pointer("/Paras/VehicleInfo/Attack").Set(document, vehicleAttack);
						auto vehicleMotility = (float)vehicleRow[4];
						Pointer("/Paras/VehicleInfo/Motility").Set(document, vehicleMotility);
						auto vehicleDefend = (float)vehicleRow[5];
						Pointer("/Paras/VehicleInfo/Defend").Set(document, vehicleDefend);
						auto vehicleMaxHealth = (float)vehicleRow[6];
						Pointer("/Paras/VehicleInfo/MaxHealth").Set(document, vehicleMaxHealth);
						auto vehiclePrice = (int)(float)vehicleRow[7];
						Pointer("/Paras/VehicleInfo/Price").Set(document, vehiclePrice);
						auto vehicleIntro = MysqlxBytes2String(vehicleRow.getBytes(8));
						Pointer("/Paras/VehicleInfo/Intro").Set(document, vehicleIntro.c_str());

						session.commit();
						player->AddVehicle(vehicleId, vehicleName, vehicleType, vehicleAttack, vehicleMotility, vehicleDefend, vehicleMaxHealth, vehiclePrice, vehicleIntro);
						player->UpdateFromAccountTable();
					}
					catch (const std::exception&)
					{
						session.rollback();
						Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
						Pointer("/Paras/Info").Set(document, "����ʧ��");
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "��Ҳ���");
				}

				SERIALIZE_DOCUMENT;
				player->SendMessageFn(output);
			}
			HANDLE_CATCH(player->SendMessageFn);
		}
		void AccountHandle::ChangeVehicle(string userId, string vehicleId, PlayerModel * player)
		{
			CONSTRUCT_DOCUMENT(Common::ChangeVehicleResult.c_str());

			try
			{
				auto session = DBUtil::GetInstance()->GetSession();
				auto accountTable = session.getDefaultSchema().getTable("account");
				auto possessVehicleTable = session.getDefaultSchema().getTable("possess_vehicle");

				//�ж��û��Ƿ�ӵ�и��ؾ�
				if (possessVehicleTable.select().where("user_id=:userId and vehicle_id=:vehicleId").bind("userId", userId).bind("vehicleId", vehicleId).execute().count() > 0)
				{
					//�ж�Ҫ�������ؾ��Ƿ��뵱ǰ�ؾ�һ��
					if (vehicleId != to_string((int)accountTable.select("cur_vehicle_id").where("user_id=:userId").limit(1).bind("userId", userId).execute().fetchOne()[0]))
					{
						if (accountTable.update().set("cur_vehicle_id", stoi(vehicleId)).where("user_id=:userId").limit(1).bind("userId", userId).execute().getAffectedItemsCount() == 1)
						{
							Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
							Pointer("/Paras/Info").Set(document, "�����ɹ�");
							Pointer("/Paras/UserInfo/CurVehicleId").Set(document, vehicleId.c_str());
							player->UpdateFromAccountTable();
						}
						else
						{
							Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
							Pointer("/Paras/Info").Set(document, "����ʧ��");
						}
					}
					else
					{
						Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
						Pointer("/Paras/Info").Set(document, "��װ�����ؾ�");
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "����ʧ��");
				}
				SERIALIZE_DOCUMENT;
				player->SendMessageFn(output);
			}
			HANDLE_CATCH(player->SendMessageFn);
		}
	}
}
