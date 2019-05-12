#include "pch.h"
#include <algorithm>
#include <random>
#include <chrono>
#include "RoomManager.h"

namespace GameServer
{
	namespace Entity
	{
		//�����ɹ����ط����ID��
		std::string RoomManager::AddRoom(std::string roomName, std::string roomMode, std::string roomMap, PlayerModel* owner)
		{
			auto roomId = PopRoomId();
			_roomMap.emplace(std::piecewise_construct, std::make_tuple(roomId), std::make_tuple(roomId, roomName, roomMode, roomMap, owner));
			return roomId;
		}
		//���ݷ���id�Ƴ�����
		void RoomManager::RemoveRoom(std::string roomId)
		{
			auto iter = _roomMap.find(roomId);
			if (iter != _roomMap.end())
			{
				_roomMap.erase(iter);
			}
			PushRoomId(roomId);
		}
		//��ȡһ�����ظ���4λId
		std::string RoomManager::PopRoomId()
		{
			auto roomId = _roomIdQueue.front();
			_roomIdQueue.pop_front();
			return roomId;
		}
		//��δʹ�õ�Id������RoomManager
		void RoomManager::PushRoomId(std::string roomId)
		{
			//ȷ�����������ͬ��Id
			if (std::find(_roomIdQueue.begin(), _roomIdQueue.end(), roomId) == _roomIdQueue.end())
			{
				_roomIdQueue.emplace_back(roomId);
			}
		}
		//���ݷ���id��ȡ����ָ�룬δ�ҵ��򷵻�nullptr
		RoomModel* RoomManager::GetRoom(std::string roomId)
		{
			auto iter = _roomMap.find(roomId);
			if (iter != _roomMap.end())
			{
				return &iter->second;
			}
			return nullptr;
		}
		//��ȡ���з����б�
		std::vector<RoomModel*> RoomManager::GetAllRoom()
		{
			vector<RoomModel*> roomList;
			for (auto& item : _roomMap)
			{
				roomList.emplace_back(&item.second);
			}
			return std::move(roomList);
		}
		RoomManager::RoomManager()
		{
			//��ʼ������id��
			vector<int> vec;
			for (int i = 1000; i < 10000; i++)
			{
				vec.emplace_back(i);
			}
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
			for (auto& i : vec)
			{
				_roomIdQueue.emplace_back(to_string(i));
			}
		}


		RoomManager::~RoomManager()
		{
		}
	}
}
