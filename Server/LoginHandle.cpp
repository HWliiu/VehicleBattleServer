#include "pch.h"
#include "LoginHandle.h"

namespace GameServer
{
	namespace Handle
	{
		void LoginHandle::Login(string username, string password, std::function<void(string)> sendMessage)
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
			auto result = accountTable.select("username", "password").where("username=:username").limit(1).bind("username", username).lockShared().execute();
			if (result.count() > 0)
			{
				auto row = result.fetchOne();
				if ((string)row[1] == password)
				{
					Pointer("/Paras/Result").Set(document, "succeed");
					Pointer("/Paras/Info").Set(document, "��¼�ɹ�");
					// TODO: ��¼��Ϣ
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
		void LoginHandle::Logout(string username, std::function<void(std::string)> sendMessage)
		{
			// TODO: ������ʽ��֤
			auto session = DBUtil::GetInstance()->GetSession();
			auto accountTable = session.getDefaultSchema().getTable("account");
		}
		void LoginHandle::Register(string username, string password, std::function<void(string)> sendMessage)
		{
			// TODO: ������ʽ��֤
			auto session = DBUtil::GetInstance()->GetSession();
			auto accountTable = session.getDefaultSchema().getTable("account");

			Document document;
			Pointer("/Command").Set(document, "RegisterResult");
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);	// TODO: ��һ��buffer��
			//�����û����Ƿ��Ѵ���
			auto result = accountTable.select("username").where("username=:username").limit(1).bind("username", username).lockShared().execute();
			if (result.count() > 0)
			{
				Pointer("/Paras/Result").Set(document, "failure");
				Pointer("/Paras/Info").Set(document, "���û����Ѵ���");
				document.Accept(writer);
				const char* output = buffer.GetString();
				sendMessage(output);
				return;
			}
			//����û�
			auto result = accountTable.insert("username", "password").values(username, password).execute();
			if (result.getAffectedItemsCount == 1)
			{
				Pointer("/Paras/Result").Set(document, "succeed");
				Pointer("/Paras/Info").Set(document, "ע��ɹ�");
				document.Accept(writer);
				const char* output = buffer.GetString();
				sendMessage(output);
			}
			else
			{
				Pointer("/Paras/Result").Set(document, "failure");
				Pointer("/Paras/Info").Set(document, "ע��ʧ��");
				document.Accept(writer);
				const char* output = buffer.GetString();
				sendMessage(output);
			}
		}
	}
}
