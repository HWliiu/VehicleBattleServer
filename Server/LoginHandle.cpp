#include "pch.h"
#include "LoginHandle.h"

namespace GameServer
{
	namespace Handle
	{
		void LoginHandle::Login(string username, string password, std::function<void(string)> sendMessage)
		{
			// TODO: 正则表达式验证
			auto session = DBUtil::GetInstance()->GetSession();
			auto accountTable = session.getDefaultSchema().getTable("account");

			Document document;
			Pointer("/Command").Set(document, "LoginResult");
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			// TODO: 查看用户是否已登录
			//验证账号密码
			auto result = accountTable.select("username", "password").where("username=:username").limit(1).bind("username", username).lockShared().execute();
			if (result.count() > 0)
			{
				auto row = result.fetchOne();
				if ((string)row[1] == password)
				{
					Pointer("/Paras/Result").Set(document, "succeed");
					Pointer("/Paras/Info").Set(document, "登录成功");
					// TODO: 记录信息
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
		void LoginHandle::Logout(string username, std::function<void(std::string)> sendMessage)
		{
			// TODO: 正则表达式验证
			auto session = DBUtil::GetInstance()->GetSession();
			auto accountTable = session.getDefaultSchema().getTable("account");
		}
		void LoginHandle::Register(string username, string password, std::function<void(string)> sendMessage)
		{
			// TODO: 正则表达式验证
			auto session = DBUtil::GetInstance()->GetSession();
			auto accountTable = session.getDefaultSchema().getTable("account");

			Document document;
			Pointer("/Command").Set(document, "RegisterResult");
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);	// TODO: 建一个buffer池
			//查找用户名是否已存在
			auto result = accountTable.select("username").where("username=:username").limit(1).bind("username", username).lockShared().execute();
			if (result.count() > 0)
			{
				Pointer("/Paras/Result").Set(document, "failure");
				Pointer("/Paras/Info").Set(document, "该用户名已存在");
				document.Accept(writer);
				const char* output = buffer.GetString();
				sendMessage(output);
				return;
			}
			//添加用户
			auto result = accountTable.insert("username", "password").values(username, password).execute();
			if (result.getAffectedItemsCount == 1)
			{
				Pointer("/Paras/Result").Set(document, "succeed");
				Pointer("/Paras/Info").Set(document, "注册成功");
				document.Accept(writer);
				const char* output = buffer.GetString();
				sendMessage(output);
			}
			else
			{
				Pointer("/Paras/Result").Set(document, "failure");
				Pointer("/Paras/Info").Set(document, "注册失败");
				document.Accept(writer);
				const char* output = buffer.GetString();
				sendMessage(output);
			}
		}
	}
}
