#pragma once
#include <iostream>
#include <functional>
#include <vector>
#include "VehicleModel.h"

namespace GameServer
{
	using namespace std;
	namespace Entity
	{
		class PlayerModel
		{
		private:
			const std::string _userId;
			const std::string _userName;
			const std::string _token;
			std::string _experience;
			std::string _money;
			std::string _level;
			std::string _curVehicleId;
			const std::string _registerTime;
			const std::string _lastLoginTime;

			vector<VehicleModel> _vehicleList;

			std::string _curRoomID;
			bool _prepareState;
			int _maxHealth;
			int _health;
			bool _isDie;
		public:
			std::function<void(std::string)> SendMessageFn;
			unsigned __int64 ConnSocket;

			PlayerModel(std::string userId, std::string userName, std::string token, std::string experience, std::string money, std::string level, std::string curVehicleId, std::string registerTime, std::string lastLoginTime, vector<VehicleModel> vehicleList, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage);
			~PlayerModel();

			inline std::string GetUserId() { return _userId; }
			inline std::string GetName() { return _userName; }
			inline std::string GetToken() { return _token; }
			//get money
			inline std::string GetLevel() { return _level; }
			//get ...time
			inline std::string GetCurRoomId() { return _curRoomID; }
			inline bool GetPrepareState() { return _prepareState; }
			inline int GetHealth() { return _health; }

			inline void TakeDamage(int value)
			{
				if (_health - value < 0)
				{
					_health = 0;
					_isDie = true;
				}
				else
				{
					_health = _health - value;
				}
			}
			inline void Repair(int value)
			{
				if (_health + value > _maxHealth)
				{
					_health = _maxHealth;
				}
				else
				{
					_health = _health + value;
				}
			}


			inline void SetCurRoomId(std::string roomId) { _curRoomID = roomId; }
			inline void SetPrepareState(bool state) { _prepareState = state; }

			void UpdateFromAccountTable();
			void AddVehicle(std::string vehicleId, std::string vehicleName, std::string vehicleType, std::string attack, std::string motility, std::string defend, std::string maxHealth, std::string price, std::string intro);
			VehicleModel* GetCurVehicle();

			//PlayerModel(const PlayerModel&) = delete;
			//PlayerModel& operator = (const PlayerModel&) = delete;

			//PlayerModel(PlayerModel&& other);
			//PlayerModel& operator = (PlayerModel&& other);
		};
	}
}

