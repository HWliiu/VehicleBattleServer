#pragma once
#include <iostream>
#include <Winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <process.h>
#include <exception>
#include <functional> 
#include <string>
#include <mutex>
#include <stack>
#include <atomic>
#include "CommandDispatcher.h"

#pragma comment(lib,"ws2_32.lib")

#if _MSC_VER >= 1600  /* 1600 is Microsoft Visual Studio 2010 */
#pragma execution_character_set("utf-8")	//指示char的执行字符集是UTF-8编码
#endif	//ps:规定网络数据传输统一使用utf-8编码


namespace GameServer
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
		char* Dequeue();
	private:
		static const int _size = DATA_BUFSIZE * 10;
		int _rear = 0;
		char _data[_size];
		std::mutex _recvBufQueueMtx;
		char DetachedMsg[DATA_BUFSIZE];
	};

	struct PerHandleData
	{
		SOCKET socket;
		SOCKADDR_IN clientAddr;
		RecvBufQueue recvMsgQueue;
		std::function<void(std::string)> sendMessage;
	};

	struct PerIoData
	{
		OVERLAPPED overlapped;
		WSABUF wsabuf;
		char buffer[DATA_BUFSIZE];
		OperatorType operatorType;	//操作类型
		SOCKET acceptSocket;	//用来存储ACCEPT类型时客户端的套接字
	};

	class FreeIoDataPool
	{
	public:
		const int FillSize;

		FreeIoDataPool(int fillSize) : FillSize(fillSize) { }
		void Push(PerIoData* lpPerIoData);
		PerIoData* Pop();
		int GetCount() { return _pool.size(); }
		void FillPool(int count);
	private:
		std::stack<PerIoData*> _pool;
		std::mutex _ioDataPoolMtx;
	};

	class IOCPServer
	{
	public:
		IOCPServer(int postAcceptCount, int freeIoDataCount, int port = 8080);
		~IOCPServer();
		void Start();
		bool Stop();
	private:
		SOCKET _listenSocket;
		HANDLE _completionPort;
		int _postAcceptCount;
		std::atomic_int _freeAcceptSockCount = { 0 };
		std::mutex _threadMtx;
		FreeIoDataPool _freeIoDataPool;
		HANDLE _postAcceptEvent;
		LPFN_ACCEPTEX _lpfnAcceptEx;
		CommandDispatcher _commandDispatcher;

		bool PostAccept();
		bool PostRecv(SOCKET socket);
		bool PostSend(SOCKET socket, std::string msg);
		void ProcessAccept(PerHandleData* lpListenSockHandleData, PerIoData* lpPerIoData);
		void ProcessRecv(PerHandleData* lpPerHandleData, PerIoData* lpPerIoData, DWORD dwBytesTransferred);
		void ProcessSend(PerHandleData* lpPerHandleData, PerIoData* lpPerIoData, DWORD dwBytesTransferred);

		static unsigned int WINAPI WorksThread(LPVOID lpParam);
	};

	// TODO: 暂时先放这里
	namespace Util
	{
		//这个函数专门用来将utf-8编码的char*字符串转换成ANSI编码以便在控制台中显示(直接改控制台的编码方式也行)
		inline char* U2G(const char* utf8)
		{
			int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
			wchar_t* wstr = new wchar_t[len + 1];
			memset(wstr, 0, len + 1);
			MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
			len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
			char* str = new char[len + 1];
			memset(str, 0, len + 1);
			WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
			if (wstr) delete[] wstr;
			return str;
		}
	}
}