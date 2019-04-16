#include "pch.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/pointer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/writer.h"
#include "LoginHandle.h"
#include "DbUtil.h"

namespace GameServer
{
	using namespace Util;
	using namespace rapidjson;
	namespace Handle
	{
		void LoginHandle::Login(std::string username, std::string password, std::function<void(std::string)> sendMessage)
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
			auto rowResult = accountTable.select("username", "password").where("username=:username").limit(1).bind("username", username).lockShared().execute();
			if (rowResult.count() > 0)
			{
				auto row = rowResult.fetchOne();
				if ((std::string)row[1] == password)
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
		void LoginHandle::Logout(std::string username, std::function<void(std::string)> sendMessage)
		{
			// TODO: 正则表达式验证
			auto session = DBUtil::GetInstance()->GetSession();
			auto accountTable = session.getDefaultSchema().getTable("account");
		}
		void LoginHandle::Register(std::string username, std::string password, std::function<void(std::string)> sendMessage)
		{
			// TODO: 正则表达式验证
			auto session = DBUtil::GetInstance()->GetSession();
			auto accountTable = session.getDefaultSchema().getTable("account");

			Document document;
			Pointer("/Command").Set(document, "RegisterResult");
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);	// TODO: 建一个buffer池
			//查找用户名是否已存在
			auto rowResult = accountTable.select("username").where("username=:username").limit(1).bind("username", username).lockShared().execute();
			if (rowResult.count() > 0)
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
			if (result.getAffectedItemsCount() == 1)
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
