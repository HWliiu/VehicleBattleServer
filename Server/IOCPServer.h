#pragma once
#include <iostream>
#include <Winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <process.h>
#include <functional>
#include <exception>
#include <string>
#include <mutex>
#include <stack>
#include <atomic>
#include "ThreadPool.h"

namespace GameServer
{
	namespace Service
	{
		constexpr auto DATA_BUFSIZE = 8192;
		enum class OperatorType
		{
			NONE,
			ACCEPT,
			RECV,
			SEND,
		};

		class RecvBufQueue
		{
		public:
			void Enqueue(char* buffer, int size);
			std::string Dequeue();
		private:
			static const int _size = DATA_BUFSIZE * 10;
			int _rear = 0;
			char _data[_size];
			std::mutex _recvBufQueueMtx;
		};

		struct PerHandleData
		{
			SOCKET socket;
			SOCKADDR_IN clientAddr;
			RecvBufQueue recvMsgQueue;
			std::function<void(std::string)> sendMessageFn;
		};

		struct PerIoData
		{
			OVERLAPPED overlapped;
			WSABUF wsabuf;
			char buffer[DATA_BUFSIZE];
			OperatorType operatorType;	//��������
			SOCKET acceptSocket;	//�����洢ACCEPT����ʱ�ͻ��˵��׽���
		};

		class FreeIoDataPool
		{
		public:
			const int FillSize;

			FreeIoDataPool(int fillSize) : FillSize(fillSize) { }
			~FreeIoDataPool();
			void Push(PerIoData* lpPerIoData);
			PerIoData* Pop();
			int GetCount() { return _pool.size(); }
			void FillPool(int count);
			void ReducePool(int count);
		private:
			std::stack<PerIoData*> _pool;
			std::mutex _ioDataPoolMtx;
		};

		class IOCPServer
		{
		public:
			IOCPServer();
			~IOCPServer();
			void Start();
			bool Stop();
		private:
			SOCKET _listenSocket;
			HANDLE _completionPort;
			std::string _address;
			int _port;
			int _postAcceptCount;
			std::atomic_int _freeAcceptSockCount;
			std::mutex _threadMtx;
			std::unique_ptr<FreeIoDataPool> _upFreeIoDataPool;
			std::unique_ptr<Common::ThreadPool> _upDispatchCmdThreadPool;
			HANDLE _postAcceptEvent;
			LPFN_ACCEPTEX _lpfnAcceptEx;

			bool PostAccept();
			bool PostRecv(SOCKET socket);
			bool PostSend(SOCKET socket, std::string msg);
			void ProcessAccept(PerHandleData* lpListenSockHandleData, PerIoData* lpPerIoData);
			void ProcessRecv(PerHandleData* lpPerHandleData, PerIoData* lpPerIoData, DWORD dwBytesTransferred);
			void ProcessSend(PerHandleData* lpPerHandleData, PerIoData* lpPerIoData, DWORD dwBytesTransferred);

			void ProcessDisconnect(PerHandleData* lpPerHandleData, PerIoData* lpPerIoData);

			static unsigned int WINAPI WorksThread(LPVOID lpParam);
		};
	}
}