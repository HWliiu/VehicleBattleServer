#include "pch.h"
#include "VehicleModel.h"

namespace GameServer
{
	namespace Entity
	{
		VehicleModel::VehicleModel(std::string vehicleId, std::string vehicleName, std::string vehicleType, std::string attack, std::string motility, std::string defend, std::string maxHealth, std::string price, std::string intro)
		{
			VehicleId = vehicleId;
			VehicleName = vehicleName;
			VehicleType = vehicleType;
			Attack = attack;
			Motility = motility;
			Defend = defend;
			MaxHealth = maxHealth;
			Price = price;
			Intro = intro;
		}
		VehicleModel::~VehicleModel()
		{
		}
	}
}
