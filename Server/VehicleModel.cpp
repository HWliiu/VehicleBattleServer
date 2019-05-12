#include "pch.h"
#include "VehicleModel.h"

namespace GameServer
{
	namespace Entity
	{
		VehicleModel::VehicleModel(std::string vehicleId, std::string vehicleName, std::string vehicleType, float attack, float motility, float defend, float maxHealth, int price, std::string intro)
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
