#pragma once
#include "include\mysqlx\xdevapi.h"
#include "Singleton.h"
#pragma  comment(lib,"lib\\mysqlcppconn8.lib") //����ļ���connector/c++ 8.0.15��x64 debug�汾

using namespace mysqlx;
namespace GameServer
{
	namespace Common
	{
		class DBUtil :public Singleton<DBUtil>
		{
		public:
			friend class Singleton<DBUtil>;
			inline Session GetSession() { return _upClient->getSession(); }
		private:
			std::unique_ptr<Client> _upClient;	//ClientΪxdevapi�ṩ��Session����

			std::string _host;
			unsigned int _port;
			std::string _user;
			std::string _pwd;
			std::string _defaultDb;
			int _poolSize;

			DBUtil()
			{
				_host = "localhost";	// TODO:�������ļ���ȡ
				_port = 33060;
				_user = "root";
				_pwd = "123";
				_defaultDb = "vehicleBattle";
				_poolSize = 32;
				_upClient = std::make_unique<Client>(
					SessionOption::USER, _user,
					SessionOption::PWD, _pwd,
					SessionOption::HOST, _host,
					SessionOption::PORT, _port,
					SessionOption::DB, _defaultDb,
					SessionOption::SSL_MODE, SSLMode::DISABLED,
					ClientOption::POOLING, true,
					ClientOption::POOL_MAX_SIZE, _poolSize,
					ClientOption::POOL_QUEUE_TIMEOUT, 1000,
					ClientOption::POOL_MAX_IDLE_TIME, 500
					);
			}
			~DBUtil()
			{
				_upClient->close();
			}
		};
	}
}

