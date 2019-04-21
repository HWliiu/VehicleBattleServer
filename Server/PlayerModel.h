#pragma once
#include <iostream>
#include <functional>
#include <vector>
#include "EquipmentModel.h"
#include "VehicleModel.h"

namespace GameServer
{
	using namespace std;
	namespace Entity
	{
		//enum class OnlineState
		//{
		//	Leisure,
		//	Teaming,
		//	Gaming,
		//	Offline
		//};

		class PlayerModel
		{
		private:
			const std::string _userId;
			const std::string _userName;
			const std::string _token;
			//int _experience;
			//int _money;
			//std::string _registerTime;
			//std::string _lastLoginTime;
			std::string _level;
			std::string _curVehicleId;
			//std::vector<VehicleModel>* _pVehicleList;
			//std::vector<EquipmentModel>* _pEquipmentList;

			std::string _curRoomID;
			bool _prepareState;
			int _maxHealth;
			int _health;
			int _isDie;
		public:
			//OnlineState OnlineState;
			std::function<void(std::string)> SendMessageFn;
			unsigned __int64 ConnSocket;

			PlayerModel(std::string userId, std::string userName, std::string token, std::string level, std::string curVehicleId, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage);
			~PlayerModel();

			inline std::string GetUserId() { return _userId; }
			inline std::string GetName() { return _userName; }
			inline std::string GetToken() { return _token; }
			inline std::string GetLevel() { return _level; }
			inline std::string GetCurVehicleId() { return _curVehicleId; }
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

			void UpdateFromDB();

			inline void SetCurRoomId(std::string roomId) { _curRoomID = roomId; }
			inline void SetPrepareState(bool state) { _prepareState = state; }


			//PlayerModel(const PlayerModel&) = delete;
			//PlayerModel& operator = (const PlayerModel&) = delete;

			//PlayerModel(PlayerModel&& other);
			//PlayerModel& operator = (PlayerModel&& other);

			//void AddExperience(int value);
			//void AddMoney(int value);
			//void ReduceMoney(int value);
			//void LevelUP();
			//void ChangeCurVehicle(std::string vehicleId);
			//void AddVehicle();
			//void RemoveVehicle();
			//void AddEquipment();
			//void RemoveEquipment();

		};
	}
}

