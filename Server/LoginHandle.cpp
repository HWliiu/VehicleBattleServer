#include "pch.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/pointer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/writer.h"
#include "LoginHandle.h"
#include "DbUtil.h"
#include "SmallTools.h"

namespace GameServer
{
	using namespace Util;
	using namespace rapidjson;
	namespace Handle
	{
		void LoginHandle::Login(std::string username, std::string password, std::function<void(std::string)> sendMessage)
		{
			// TODO: ������ʽ��֤
			auto session = DBUtil::GetInstance()->GetSession();
			auto accountTable = session.getDefaultSchema().getTable("account");

			Document document;
			Pointer("/Command").Set(document, "LoginResult");
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			// TODO: �鿴�û��Ƿ��ѵ�¼

			//��֤�˺�����
			auto rowResult = accountTable.select("username", "password").where("username=:username").limit(1).bind("username", username).lockShared().execute();
			if (rowResult.count() > 0)
			{
				auto row = rowResult.fetchOne();
				if ((std::string)row[1] == password)
				{
					Pointer("/Paras/Result").Set(document, "succeed");
					Pointer("/Paras/Info").Set(document, "��¼�ɹ�");
					// TODO: ��¼��Ϣ
					accountTable.update().set("last_login_time", GetCurTime()).where("username=:username").limit(1).bind("username", username).execute();


					document.Accept(writer);
					const char* output = buffer.GetString();
					sendMessage(output);
				}
				else
				{
					Pointer("/Paras/Result").Set(document, "failure");
					Pointer("/Paras/Info").Set(document, "�������");
					document.Accept(writer);
					const char* output = buffer.GetString();
					sendMessage(output);
				}
			}
			else
			{
				Pointer("/Paras/Result").Set(document, "failure");
				Pointer("/Paras/Info").Set(document, "���û�������");
				document.Accept(writer);
				const char* output = buffer.GetString();
				sendMessage(output);
			}
		}
		void LoginHandle::Logout(std::string username, std::function<void(std::string)> sendMessage)
		{
			// TODO: ������ʽ��֤
			auto session = DBUtil::GetInstance()->GetSession();
			auto accountTable = session.getDefaultSchema().getTable("account");
		}
		void LoginHandle::Register(std::string username, std::string password, std::function<void(std::string)> sendMessage)
		{
			// TODO: ������ʽ��֤
			auto session = DBUtil::GetInstance()->GetSession();
			auto accountTable = session.getDefaultSchema().getTable("account");

			Document document;
			Pointer("/Command").Set(document, "RegisterResult");
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);	// TODO: ��һ��buffer��
			//�����û����Ƿ��Ѵ���
			auto userRowResult = accountTable.select("username").where("username=:username").limit(1).bind("username", username).lockShared().execute();
			if (userRowResult.count() > 0)
			{
				Pointer("/Paras/Result").Set(document, "failure");
				Pointer("/Paras/Info").Set(document, "���û����Ѵ���");
				document.Accept(writer);
				const char* output = buffer.GetString();
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

				Pointer("/Paras/Result").Set(document, "succeed");
				Pointer("/Paras/Info").Set(document, "ע��ɹ�");
				document.Accept(writer);
				const char* output = buffer.GetString();
				sendMessage(output);
			}
			else
			{
				session.rollback();
				Pointer("/Paras/Result").Set(document, "failure");
				Pointer("/Paras/Info").Set(document, "ע��ʧ��");
				document.Accept(writer);
				const char* output = buffer.GetString();
				sendMessage(output);
			}
		}
	}
}
