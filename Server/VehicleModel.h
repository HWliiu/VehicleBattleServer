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
			float Attack;
			float Motility;
			float Defend;
			float MaxHealth;
			int Price;
			std::string Intro;
			VehicleModel(std::string vehicleId, std::string vehicleName, std::string vehicleType, float attack, float motility, float defend, float maxHealth, int price, std::string intro);
			~VehicleModel();
		};
	}
}

