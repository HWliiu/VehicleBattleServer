#pragma once
#include <iostream>
namespace GameServer
{
	namespace Entity
	{
		class PlayerData
		{
		public:
			std::string UserID;
			std::string Username;
			std::string Experience;
			std::string Money;
			std::string Level;

			PlayerData();
			~PlayerData();

			PlayerData(const PlayerData&) = delete;
			PlayerData& operator = (const PlayerData&) = delete; //禁止使用默认的拷贝构造函数和赋值构造函数

			PlayerData(PlayerData&& other);
			PlayerData& operator = (PlayerData&& other);
		};
	}
}

