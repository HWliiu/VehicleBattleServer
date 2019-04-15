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
			PlayerData& operator = (const PlayerData&) = delete; //��ֹʹ��Ĭ�ϵĿ������캯���͸�ֵ���캯��

			PlayerData(PlayerData&& other);
			PlayerData& operator = (PlayerData&& other);
		};
	}
}

