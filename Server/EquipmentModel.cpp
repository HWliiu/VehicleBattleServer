#include "pch.h"
#include "EquipmentModel.h"

namespace GameServer
{
	namespace Entity
	{
		EquipmentModel::EquipmentModel(std::string equipmentId, std::string equipmentName, std::string equipmentType, int level, int effectValue, int duration, int coolingTime, int price, std::string intro)
		{
			EquipmentId = equipmentId;
			EquipmentName = equipmentName;
			EquipmentType = equipmentType;
			Level = level;
			EffectValue = effectValue;
			Duration = duration;
			CoolingTime = coolingTime;
			Price = price;
			Intro = intro;
		}
		EquipmentModel::~EquipmentModel()
		{
		}
	}
}
