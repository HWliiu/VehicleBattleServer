#pragma once
#include <iostream>
namespace GameServer
{
	namespace Entity
	{
		using std::string;
		class PlayerData
		{
		public:
			string UserID;
			string Username;
			string Experience;
			string Money;
			string Level;

			PlayerData();
			~PlayerData();

			PlayerData(const PlayerData&) = delete;
			PlayerData& operator = (const PlayerData&) = delete; //��ֹʹ��Ĭ�ϵĿ������캯���͸�ֵ���캯��

			PlayerData(PlayerData&& other);
			PlayerData& operator = (PlayerData&& other);
		};
	}
}

