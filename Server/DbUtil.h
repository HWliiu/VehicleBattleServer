#pragma once
#include "include\mysqlx\xdevapi.h"
#include "Singleton.h"
#pragma  comment(lib,"lib\\mysqlcppconn8.lib") //����ļ�����vs2019�����x64 debug�汾

using namespace mysqlx;
namespace GameServer
{
	namespace Util
	{
		class DBUtil :public Singleton<DBUtil>
		{
		public:
			friend class Singleton<DBUtil>;
			Session GetSession();
		private:
			Client* _client;	//ClientΪxdevapi�ṩ��Session����

			std::string _host;
			unsigned int _port;
			std::string _user;
			std::string _pwd;
			std::string _defaultDb;
			int _poolSize;

			DBUtil();
			~DBUtil();
		};
	}
}

