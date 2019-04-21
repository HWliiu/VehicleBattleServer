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
			int Attack;
			int Motility;
			int Defend;
			int MaxHealth;
			int Price;
			std::string Intro;
			VehicleModel(std::string vehicleId, std::string vehicleName, std::string vehicleType, int attack, int motility, int defend, int maxHealth, int price, std::string intro);
			~VehicleModel();
		};
	}
}

