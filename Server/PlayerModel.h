#pragma once
#include <iostream>
namespace GameServer
{
	namespace Entity
	{
		class PlayerModel
		{
		public:
			std::string UserID;
			std::string UserName;
			std::string Experience;
			std::string Money;
			std::string Level;
			std::string RegisterTime;
			std::string LastLoginTime;
			//curVehicle
			//vehicleList
			//equipmentList
			std::string CurRoomID;
			bool PrepareState;
			OnlineState OnlineState;
			int Haelth;
			std::function<void(std::string)> SendMessageFn;

			PlayerModel();
			~PlayerModel();

			PlayerModel(const PlayerModel&) = delete;
			PlayerModel& operator = (const PlayerModel&) = delete; //禁止使用默认的拷贝构造函数和赋值构造函数

			PlayerModel(PlayerModel&& other);
			PlayerModel& operator = (PlayerModel&& other);

			void AddExperience(int value);
			void AddMoney(int value);
			void ReduceMoney(int value);
			void LevelUP();
			void ChangeCurVehicle(std::string vehicleId);
			void AddVehicle();
			void RemoveVehicle();
			void AddEquipment();
			void RemoveEquipment();
			void SetCurRoomID(std::string roomId);

		};

		enum class OnlineState
		{
			Leisure,	//在大厅
			Teaming,	//正在组队
			Gaming,		//正在游玩
			Offline
		};
	}
}

