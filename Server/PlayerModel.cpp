#include "pch.h"
#include "PlayerModel.h"
#include "DbUtil.h"

namespace GameServer
{
	namespace Entity
	{
		PlayerModel::PlayerModel(std::string userId, std::string userName, std::string token, std::string level, std::string curVehicleId, unsigned __int64 connSocket, std::function<void(std::string)> sendMessage) :_userId(userId), _userName(userName), _token(token)
		{
			_level = level;
			_curVehicleId = curVehicleId;
			ConnSocket = connSocket;
			SendMessageFn = sendMessage;
			_curRoomID = "0";
		}
		PlayerModel::~PlayerModel()
		{
		}
		void PlayerModel::UpdateFromDB()
		{
		}
	}
}
