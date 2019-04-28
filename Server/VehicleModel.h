#pragma once
#include <string>

namespace GameServer
{
	namespace Entity
	{
		class VehicleModel
		{
		public:
			std::string VehicleId;
			std::string VehicleName;
			std::string VehicleType;
			std::string Attack;
			std::string Motility;
			std::string Defend;
			std::string MaxHealth;
			std::string Price;
			std::string Intro;
			VehicleModel(std::string vehicleId, std::string vehicleName, std::string vehicleType, std::string attack, std::string motility, std::string defend, std::string maxHealth, std::string price, std::string intro);
			~VehicleModel();
		};
	}
}

