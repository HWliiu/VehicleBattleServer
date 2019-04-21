#pragma once
#include <string>

namespace GameServer
{
	namespace Entity
	{
		//���߹��ܲ���������������������
		class EquipmentModel
		{
		public:
			std::string EquipmentId;
			std::string EquipmentName;
			std::string EquipmentType;
			int Level;
			int EffectValue;
			int Duration;
			int CoolingTime;
			int Price;
			int Num;
			std::string Intro;

			EquipmentModel(std::string equipmentId, std::string equipmentName, std::string equipmentType, int level, int effectValue, int duration, int coolingTime, int price, std::string intro);
			~EquipmentModel();
		};
	}
}

