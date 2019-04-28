#include "pch.h"
#include "PlayerModel.h"
#include "DbUtil.h"

namespace GameServer
{
	namespace Entity
	{
		PlayerModel::PlayerModel(std::string userId, std::string userName, std::string token, std::string experience, std::string money, std::string level, std::string curVehicleId, std::string registerTime, std::string lastLoginTime, vector<VehicleModel> vehicleList, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage) :_userId(userId), _userName(userName), _token(token), _registerTime(registerTime), _lastLoginTime(lastLoginTime), _vehicleList(vehicleList)
		{
			_experience = experience;
			_money = money;
			_level = level;
			_curVehicleId = curVehicleId;
			ConnSocket = connSocket;
			SendMessageFn = sendMessage;

			_curRoomID = "";
			_prepareState = false;
			_maxHealth = stoi(GetCurVehicle()->MaxHealth);
			_health = _maxHealth;
			_isDie = false;
		}
		PlayerModel::~PlayerModel()
		{
		}
		void PlayerModel::UpdateFromAccountTable()
		{
			auto session = Common::DBUtil::GetInstance()->GetSession();
			auto accountTable = session.getDefaultSchema().getTable("account");

			auto accountRowResult = accountTable.select().where("user_id=:userId").limit(1).bind("userId", _userId).execute();
			if (accountRowResult.count() > 0)
			{
				auto accountRow = accountRowResult.fetchOne();

				_experience = to_string((int)accountRow[3]);
				_money = to_string((int)accountRow[4]);
				_level = to_string((int)accountRow[5]);
				_curVehicleId = to_string((int)accountRow[6]);
			}
		}
		void PlayerModel::AddVehicle(std::string vehicleId, std::string vehicleName, std::string vehicleType, std::string attack, std::string motility, std::string defend, std::string maxHealth, std::string price, std::string intro)
		{
			_vehicleList.emplace_back(vehicleId, vehicleName, vehicleType, attack, motility, defend, maxHealth, price, intro);
		}
		VehicleModel* PlayerModel::GetCurVehicle()
		{
			for (auto& vehicle : _vehicleList)
			{
				if (vehicle.VehicleId == _curVehicleId)
				{
					return &vehicle;
				}
			}
			return nullptr;
		}
	}
}