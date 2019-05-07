#include "pch.h"
#include "StoreHandle.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/pointer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/writer.h"
#include "DbUtil.h"
#include "SmallTools.h"
#include "PlayerManager.h"
#include "Consts.h"

namespace GameServer
{
	using namespace Common;
	using namespace Entity;
	using namespace rapidjson;
	namespace Handle
	{
		void StoreHandle::ListStoreItem(std::string userId, PlayerModel* player)
		{
			PRE_HANDLE(Common::StoreItemListResult.c_str());

			try
			{
				auto session = DBUtil::GetInstance()->GetSession();
				auto vehicleTable = session.getDefaultSchema().getTable("vehicle");

				auto vehicleRowResult = vehicleTable.select().execute();
				Row vehicleRow;
				int i = 0;
				while (vehicleRow = vehicleRowResult.fetchOne())
				{
					char path[256];
					sprintf_s(path, "/Paras/VehicleInfo/%d/Id", i);
					Pointer(path).Set(document, to_string((int)vehicleRow[0]).c_str());
					sprintf_s(path, "/Paras/VehicleInfo/%d/Name", i);
					Pointer(path).Set(document, MysqlxBytes2String(vehicleRow.getBytes(1)).c_str());
					sprintf_s(path, "/Paras/VehicleInfo/%d/Type", i);
					Pointer(path).Set(document, ((std::string)vehicleRow[2]).c_str());
					sprintf_s(path, "/Paras/VehicleInfo/%d/Attack", i);
					Pointer(path).Set(document, to_string((float)vehicleRow[3]).c_str());
					sprintf_s(path, "/Paras/VehicleInfo/%d/Motility", i);
					Pointer(path).Set(document, to_string((float)vehicleRow[4]).c_str());
					sprintf_s(path, "/Paras/VehicleInfo/%d/Defend", i);
					Pointer(path).Set(document, to_string((float)vehicleRow[5]).c_str());
					sprintf_s(path, "/Paras/VehicleInfo/%d/MaxHealth", i);
					Pointer(path).Set(document, to_string((float)vehicleRow[6]).c_str());
					sprintf_s(path, "/Paras/VehicleInfo/%d/Price", i);
					Pointer(path).Set(document, to_string((float)vehicleRow[7]).c_str());
					sprintf_s(path, "/Paras/VehicleInfo/%d/Intro", i);
					Pointer(path).Set(document, MysqlxBytes2String(vehicleRow.getBytes(8)).c_str());
					i++;
				}
				Pointer("/Paras/Result").Set(document, Common::SUCCEED.c_str());
				Pointer("/Paras/Info").Set(document, "");
				SENDMESSAGE2;
			}
			HANDLE_CATCH2

		}
	}
}
