#include "pch.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/pointer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/writer.h"
#include "LoginHandle.h"
#include "DbUtil.h"
#include "SmallTools.h"
#include "PlayerManager.h"

namespace GameServer
{
	using namespace Util;
	using namespace Entity;
	using namespace rapidjson;
	namespace Handle
	{
		void LoginHandle::Login(std::string username, std::string password, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
			auto session = DBUtil::GetInstance()->GetSession();
			auto accountTable = session.getDefaultSchema().getTable("account");

			Document document;
			Pointer("/Command").Set(document, "LoginResult");
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			//获取用户信息
			auto accountRowResult = accountTable.select().where("username=:username").limit(1).bind("username", username).lockShared().execute();
			if (accountRowResult.count() > 0)
			{
				auto accountRow = accountRowResult.fetchOne();
				auto id = to_string((int)accountRow[0]);
				auto username = (std::string)accountRow[1];
				auto pwd = (std::string)accountRow[2];
				auto experience = to_string((int)accountRow[3]);
				auto money = to_string((int)accountRow[4]);
				auto level = to_string((int)accountRow[5]);
				auto curVehicleId = to_string((int)accountRow[6]);
				auto registerTime = accountRow[7].isNull() ? "" : (std::string)accountRow[7];
				auto lastLoginTime = accountRow[8].isNull() ? "" : (std::string)accountRow[8];
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
							//Logout(player->GetUserId(), player->ConnSocket, player->SendMessageFn);
							PlayerManager::GetInstance()->RemovePlayer(id);
							closesocket(player->ConnSocket);	//只在这里服务器主动关闭套接字
						}
						else
						{
							Pointer("/Paras/Result").Set(document, "succeed");
							Pointer("/Paras/Info").Set(document, "您已登录 无需重复登录");
							document.Accept(writer);
							const char* output = buffer.GetString();
							sendMessage(output);
							return;
						}
					}
					std::string token = GenerateGuid();
					Pointer("/Paras/Result").Set(document, "succeed");
					Pointer("/Paras/Info").Set(document, "登录成功");
					Pointer("/Paras/UserInfo/Id").Set(document, id.c_str());
					Pointer("/Paras/UserInfo/UserName").Set(document, username.c_str());
					Pointer("/Paras/UserInfo/Token").Set(document, token.c_str());
					Pointer("/Paras/UserInfo/Experience").Set(document, experience.c_str());
					Pointer("/Paras/UserInfo/Money").Set(document, money.c_str());
					Pointer("/Paras/UserInfo/Level").Set(document, level.c_str());
					Pointer("/Paras/UserInfo/CurVehicleId").Set(document, curVehicleId.c_str());
					/*Pointer("/Paras/UserInfo/RegisterTime").Set(document, registerTime.c_str());
					Pointer("/Paras/UserInfo/LastLoginTime").Set(document, lastLoginTime.c_str());*/
					//获取装备信息
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
					for (size_t i = 0; i < count; i++)
					{
						Row vehicleRow = vehicleResult.fetchOne();
						char path[256];
						sprintf_s(path, "/Paras/UserInfo/VehicleList/%d/Id", i);
						Pointer(path).Set(document, to_string((int)vehicleRow[0]).c_str());
						sprintf_s(path, "/Paras/UserInfo/VehicleList/%d/Name", i);
						Pointer(path).Set(document, MysqlxBytes2String(vehicleRow.getBytes(1)).c_str());	//中文字符串直接获取会抛异常
						sprintf_s(path, "/Paras/UserInfo/VehicleList/%d/Type", i);
						Pointer(path).Set(document, ((std::string)vehicleRow[2]).c_str());
						sprintf_s(path, "/Paras/UserInfo/VehicleList/%d/Attack", i);
						Pointer(path).Set(document, to_string((float)vehicleRow[3]).c_str());
						sprintf_s(path, "/Paras/UserInfo/VehicleList/%d/Motility", i);
						Pointer(path).Set(document, to_string((float)vehicleRow[4]).c_str());
						sprintf_s(path, "/Paras/UserInfo/VehicleList/%d/Defend", i);
						Pointer(path).Set(document, to_string((float)vehicleRow[5]).c_str());
						sprintf_s(path, "/Paras/UserInfo/VehicleList/%d/Health", i);
						Pointer(path).Set(document, to_string((float)vehicleRow[6]).c_str());
						sprintf_s(path, "/Paras/UserInfo/VehicleList/%d/Price", i);
						Pointer(path).Set(document, to_string((float)vehicleRow[7]).c_str());
						sprintf_s(path, "/Paras/UserInfo/VehicleList/%d/Intro", i);
						Pointer(path).Set(document, MysqlxBytes2String(vehicleRow.getBytes(8)).c_str());
					}
					PlayerManager::GetInstance()->AddPlayer(id, username, token, level, curVehicleId, connSocket, sendMessage);

					accountTable.update().set("last_login_time", GetCurTime()).where("username=:username").limit(1).bind("username", username).execute();

					document.Accept(writer);
					const char* output = buffer.GetString();
					sendMessage(output);
				}
				else
				{
					Pointer("/Paras/Result").Set(document, "failure");
					Pointer("/Paras/Info").Set(document, "密码错误");
					document.Accept(writer);
					const char* output = buffer.GetString();
					sendMessage(output);
				}
			}
			else
			{
				Pointer("/Paras/Result").Set(document, "failure");
				Pointer("/Paras/Info").Set(document, "该用户不存在");
				document.Accept(writer);
				const char* output = buffer.GetString();
				sendMessage(output);
			}
		}
		void LoginHandle::Logout(std::string userId, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
			Document document;
			Pointer("/Command").Set(document, "LogoutResult");
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			if (PlayerManager::GetInstance()->GetPlayer(userId) != nullptr)
			{
				PlayerManager::GetInstance()->RemovePlayer(userId);
				Pointer("/Paras/Result").Set(document, "succeed");
				Pointer("/Paras/Info").Set(document, "您已退出登录");
			}
			else
			{
				//该用户已下线
				Pointer("/Paras/Result").Set(document, "failure");
				Pointer("/Paras/Info").Set(document, "该用户已下线");
			}
			document.Accept(writer);
			const char* output = buffer.GetString();
			sendMessage(output);
		}
		void LoginHandle::Register(std::string username, std::string password, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage)
		{
			auto session = DBUtil::GetInstance()->GetSession();
			auto accountTable = session.getDefaultSchema().getTable("account");

			Document document;
			Pointer("/Command").Set(document, "RegisterResult");
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			//查找用户名是否已存在
			auto userRowResult = accountTable.select("username").where("username=:username").limit(1).bind("username", username).lockShared().execute();
			if (userRowResult.count() > 0)
			{
				Pointer("/Paras/Result").Set(document, "failure");
				Pointer("/Paras/Info").Set(document, "该用户名已存在");
				document.Accept(writer);
				const char* output = buffer.GetString();
				sendMessage(output);
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

				Pointer("/Paras/Result").Set(document, "succeed");
				Pointer("/Paras/Info").Set(document, "注册成功");
				document.Accept(writer);
				const char* output = buffer.GetString();
				sendMessage(output);
			}
			else
			{
				session.rollback();
				Pointer("/Paras/Result").Set(document, "failure");
				Pointer("/Paras/Info").Set(document, "注册失败");
				document.Accept(writer);
				const char* output = buffer.GetString();
				sendMessage(output);
			}
		}
	}
}
