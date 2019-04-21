#include "pch.h"
#include "DBUtil.h"

namespace GameServer
{
	namespace Util
	{
		Session DBUtil::GetSession()
		{
			return _client->getSession();
		}

		DBUtil::DBUtil()
		{
			_host = "localhost";	// TODO:从配置文件获取
			_port = 33060;
			_user = "root";
			_pwd = "123";
			_defaultDb = "vehicleBattle";
			_poolSize = 32;
			_client = new Client(
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


		DBUtil::~DBUtil()
		{
			_client->close();
			delete _client;
			_client = nullptr;
		}
	}
}
