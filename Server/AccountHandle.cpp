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
			PRE_HANDLE(Common::LoginResult.c_str());

			try
			{
				auto session = DBUtil::GetInstance()->GetSession();
				auto accountTable = session.getDefaultSchema().getTable("account");

				auto accountRowResult = accountTable.select().where("username=:username").limit(1).bind("username", username).execute();
				if (accountRowResult.count() > 0)
				{
					//获取用户信息
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
					//验证密码
					if (pwd == password)
					{
						//查看用户是否已登录
						if (PlayerManager::GetInstance()->GetPlayer(id) != nullptr)
						{
							auto player = PlayerManager::GetInstance()->GetPlayer(id);
							if (connSocket != player->ConnSocket)	//多设备登录
							{
								//下线并重新登录
								//Logout();
								PlayerManager::GetInstance()->RemovePlayer(id);
								closesocket(player->ConnSocket);	//只在这里以及客户端意外断线时服务器主动关闭套接字
							}
							else
							{
								Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
								Pointer("/Paras/Info").Set(document, "您已登录 无需重复登录");
								SENDMESSAGE1;
								return;
							}
						}
						std::string token = GenerateGuid();	//生成guid用于身份验证
						Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
						Pointer("/Paras/Info").Set(document, "登录成功");
						Pointer("/Paras/UserInfo/Id").Set(document, id.c_str());
						Pointer("/Paras/UserInfo/UserName").Set(document, username.c_str());
						Pointer("/Paras/UserInfo/Token").Set(document, token.c_str());
						Pointer("/Paras/UserInfo/Experience").Set(document, experience.c_str());
						Pointer("/Paras/UserInfo/Money").Set(document, money.c_str());
						Pointer("/Paras/UserInfo/Level").Set(document, level.c_str());
						Pointer("/Paras/UserInfo/CurVehicleId").Set(document, curVehicleId.c_str());
						Pointer("/Paras/UserInfo/RegisterTime").Set(document, registerTime.c_str());
						Pointer("/Paras/UserInfo/LastLoginTime").Set(document, lastLoginTime.c_str());

						//获取载具信息
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
							auto vehicleName = MysqlxBytes2String(vehicleRow.getBytes(1));	//中文字符串直接获取会抛异常
							Pointer(path).Set(document, vehicleName.c_str());

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Type", i);
							auto vehicleType = (std::string)vehicleRow[2];
							Pointer(path).Set(document, vehicleType.c_str());

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Attack", i);
							auto vehicleAttack = to_string((float)vehicleRow[3]);
							Pointer(path).Set(document, vehicleAttack.c_str());

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Motility", i);
							auto vehicleMotility = to_string((float)vehicleRow[4]);
							Pointer(path).Set(document, vehicleMotility.c_str());

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Defend", i);
							auto vehicleDefend = to_string((float)vehicleRow[5]);
							Pointer(path).Set(document, vehicleDefend.c_str());

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/MaxHealth", i);
							auto vehicleMaxHealth = to_string((float)vehicleRow[6]);
							Pointer(path).Set(document, vehicleMaxHealth.c_str());

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Price", i);
							auto vehiclePrice = to_string((float)vehicleRow[7]);
							Pointer(path).Set(document, vehiclePrice.c_str());

							sprintf_s(path, "/Paras/UserInfo/VehicleList/%zd/Intro", i);
							auto vehicleIntro = MysqlxBytes2String(vehicleRow.getBytes(8));
							Pointer(path).Set(document, vehicleIntro.c_str());

							vehicleList.emplace_back(vehicleId, vehicleName, vehicleType, vehicleAttack, vehicleMotility, vehicleDefend, vehicleMaxHealth, vehiclePrice, vehicleIntro);
						}
						//添加用户
						PlayerManager::GetInstance()->AddPlayer(id, username, token, experience, money, level, curVehicleId, registerTime, lastLoginTime, vehicleList, connSocket, sendMessage);
						//更新登录时间
						accountTable.update().set("last_login_time", GetCurTime()).where("username=:username").limit(1).bind("username", username).execute();

						SENDMESSAGE1;
					}
					else
					{
						Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
						Pointer("/Paras/Info").Set(document, "密码错误");
						SENDMESSAGE1;
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "该用户不存在");
					SENDMESSAGE1;
				}
			}
			HANDLE_CATCH1
		}

		void AccountHandle::Register(std::string username, std::string password, std::function<void(std::string)> sendMessage)
		{
			PRE_HANDLE(Common::RegisterResult.c_str());

			try
			{
				auto session = DBUtil::GetInstance()->GetSession();
				auto accountTable = session.getDefaultSchema().getTable("account");

				//查找用户名是否已存在
				auto userRowResult = accountTable.select("username").where("username=:username").limit(1).bind("username", username).execute();
				if (userRowResult.count() > 0)
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "该用户名已存在");
					SENDMESSAGE1;
					return;
				}
				//添加用户
				session.startTransaction();
				auto insertResult = accountTable.insert("username", "password", "register_time").values(username, password, GetCurTime()).execute();
				if (insertResult.getAffectedItemsCount() == 1)
				{
					//添加初始信息
					int userId = accountTable.select("user_id").where("username=:username").limit(1).bind("username", username).execute().fetchOne()[0];
					auto possessVehicle = session.getDefaultSchema().getTable("possess_vehicle");
					possessVehicle.insert("user_id", "vehicle_id").values(userId, 1).execute();	//默认拥有id为1的载具
					session.commit();

					Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
					Pointer("/Paras/Info").Set(document, "注册成功");
					SENDMESSAGE1;
				}
				else
				{
					session.rollback();
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "注册失败");
					SENDMESSAGE1;
				}
			}
			HANDLE_CATCH1
		}

		void AccountHandle::Logout(string userId, PlayerModel* player)
		{
			PRE_HANDLE(Common::LogoutResult.c_str());

			auto sendMessage = player->SendMessageFn;
			if (PlayerManager::GetInstance()->GetPlayer(userId) != nullptr)
			{
				PlayerManager::GetInstance()->RemovePlayer(userId);
				Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
				Pointer("/Paras/Info").Set(document, "成功退出登录");
			}
			else
			{
				//该用户已下线
				Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
				Pointer("/Paras/Info").Set(document, "该用户已下线");
			}
			SENDMESSAGE1;
		}

		void AccountHandle::ChangePassword(string userId, string oldPassword, string newPassword, PlayerModel* player)
		{
			PRE_HANDLE(Common::ChangePasswordResult.c_str());

			try
			{
				auto session = DBUtil::GetInstance()->GetSession();
				auto accountTable = session.getDefaultSchema().getTable("account");
				auto sendMessage = player->SendMessageFn;

				//验证密码
				auto accountRowResult = accountTable.select("password").where("user_id=:userId").limit(1).bind("userId", userId).execute();
				if ((std::string)(accountRowResult.fetchOne().get(0)) == oldPassword)
				{
					auto result = accountTable.update().set("password", newPassword).where("user_id=:userId").limit(1).bind("userId", userId).execute();
					if (result.getAffectedItemsCount() == 1)
					{
						Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
						Pointer("/Paras/Info").Set(document, "修改成功");
						//退出登录
						/*Logout();*/
						PlayerManager::GetInstance()->RemovePlayer(userId);
					}
					else
					{
						Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
						Pointer("/Paras/Info").Set(document, "修改失败");
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "原密码不正确");
				}
				SENDMESSAGE1;
			}
			HANDLE_CATCH2
		}
		void AccountHandle::PurchaseVehicle(string userId, string vehicleId, PlayerModel* player)
		{
			PRE_HANDLE(Common::PurchaseItemResult.c_str());

			try
			{
				auto session = DBUtil::GetInstance()->GetSession();
				auto accountTable = session.getDefaultSchema().getTable("account");
				auto vehicleTable = session.getDefaultSchema().getTable("vehicle");
				auto possessVehicleTable = session.getDefaultSchema().getTable("possess_vehicle");

				if (possessVehicleTable.select().where("user_id=:userId and vehicle_id=:vehicleId").bind("userId", userId).bind("vehicleId", vehicleId).execute().count() > 0)
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "您已拥有此物品");
					SENDMESSAGE2;
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
						Pointer("/Paras/Info").Set(document, "购买成功");
						Pointer("/Paras/UserInfo/Money").Set(document, to_string(userMoney - vehiclePrice).c_str());

						Row vehicleRow = vehicleTable.select().where("vehicle_id=:vehicleId").limit(1).bind("vehicleId", vehicleId).execute().fetchOne();
						auto vehicleId = to_string((int)vehicleRow[0]);
						Pointer("/Paras/VehicleInfo/Id").Set(document, vehicleId.c_str());
						auto vehicleName = MysqlxBytes2String(vehicleRow.getBytes(1));
						Pointer("/Paras/VehicleInfo/Name").Set(document, vehicleName.c_str());
						auto vehicleType = (std::string)vehicleRow[2];
						Pointer("/Paras/VehicleInfo/Type").Set(document, vehicleType.c_str());
						auto vehicleAttack = to_string((float)vehicleRow[3]);
						Pointer("/Paras/VehicleInfo/Attack").Set(document, vehicleAttack.c_str());
						auto vehicleMotility = to_string((float)vehicleRow[4]);
						Pointer("/Paras/VehicleInfo/Motility").Set(document, vehicleMotility.c_str());
						auto vehicleDefend = to_string((float)vehicleRow[5]);
						Pointer("/Paras/VehicleInfo/Defend").Set(document, vehicleDefend.c_str());
						auto vehicleMaxHealth = to_string((float)vehicleRow[6]);
						Pointer("/Paras/VehicleInfo/MaxHealth").Set(document, vehicleMaxHealth.c_str());
						auto vehiclePrice = to_string((float)vehicleRow[7]);
						Pointer("/Paras/VehicleInfo/Price").Set(document, vehiclePrice.c_str());
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
						Pointer("/Paras/Info").Set(document, "购买失败");
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "金币不足");
				}

				SENDMESSAGE2;
			}
			HANDLE_CATCH2
		}
		void AccountHandle::ChangeVehicle(string userId, string vehicleId, PlayerModel * player)
		{
			PRE_HANDLE(Common::ChangeVehicleResult.c_str());

			try
			{
				auto session = DBUtil::GetInstance()->GetSession();
				auto accountTable = session.getDefaultSchema().getTable("account");
				auto possessVehicleTable = session.getDefaultSchema().getTable("possess_vehicle");

				//判断用户是否拥有该载具
				if (possessVehicleTable.select().where("user_id=:userId and vehicle_id=:vehicleId").bind("userId", userId).bind("vehicleId", vehicleId).execute().count() > 0)
				{
					//判断要更换的载具是否与当前载具一样
					if (vehicleId != to_string((int)accountTable.select("cur_vehicle_id").where("user_id=:userId").limit(1).bind("userId", userId).execute().fetchOne()[0]))
					{
						if (accountTable.update().set("cur_vehicle_id", stoi(vehicleId)).where("user_id=:userId").limit(1).bind("userId", userId).execute().getAffectedItemsCount() == 1)
						{
							Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
							Pointer("/Paras/Info").Set(document, "更换成功");
							Pointer("/Paras/UserInfo/CurVehicleId").Set(document, vehicleId.c_str());
							player->UpdateFromAccountTable();
						}
						else
						{
							Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
							Pointer("/Paras/Info").Set(document, "更换失败");
						}
					}
					else
					{
						Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
						Pointer("/Paras/Info").Set(document, "已装备该载具");
					}
				}
				else
				{
					Pointer("/Paras/Result").Set(document, Common::FAILURE.c_str());
					Pointer("/Paras/Info").Set(document, "更换失败");
				}
				SENDMESSAGE2;
			}
			HANDLE_CATCH2
		}
	}
}
