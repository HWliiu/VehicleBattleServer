#pragma once
#include "include\mysqlx\xdevapi.h"
#include "Singleton.h"
#pragma  comment(lib,"lib\\mysqlcppconn8.lib") //这个文件是用vs2019编译的x64 debug版本

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
			Client* _client;	//Client为xdevapi提供的Session池类

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

