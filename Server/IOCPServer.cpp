#include "pch.h"
#include "IOCPServer.h"
#include "CommandDispatcher.h"
#include "SmallTools.h"

namespace GameServer
{
	namespace Service
	{
		IOCPServer::IOCPServer(int postAcceptCount, int freeIoDataCount, int port) : _postAcceptCount(postAcceptCount), _freeIoDataPool(freeIoDataCount), _dispatchCmdThreadPool(4)
		{
			//初始化socket
			WORD versionRequest = MAKEWORD(2, 2);
			WSADATA wsaData;
			if (WSAStartup(versionRequest, &wsaData) != 0)	//返回值为0代表成功 
			{
				std::cout << Util::U2G("初始化socket失败！错误代码：") << WSAGetLastError() << std::endl;
				throw std::bad_exception();
			}
			//初始化地址
			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.S_un.S_addr = INADDR_ANY;
			addr.sin_port = htons(port);
			//创建监听socket
			_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (_listenSocket == INVALID_SOCKET)
			{
				std::cout << Util::U2G("listenSocket创建失败！错误代码：") << WSAGetLastError() << std::endl;
				throw std::bad_exception();
			}
			//绑定监听socket
			if (bind(_listenSocket, (sockaddr*)& addr, sizeof(addr)) == SOCKET_ERROR)
			{
				std::cout << Util::U2G("绑定listenSocket失败！错误代码：") << WSAGetLastError() << std::endl;
				throw std::bad_exception();
			}
			//创建io完成端口
			_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
			if (_completionPort == NULL)
			{
				std::cout << Util::U2G("创建completionPort失败！错误代码：") << GetLastError() << std::endl;
				throw std::bad_exception();
			}
			//将监听socket绑定到io完成端口
			PerHandleData* lpPerHandleData = new PerHandleData();
			lpPerHandleData->socket = _listenSocket;
			if (CreateIoCompletionPort((HANDLE)_listenSocket, _completionPort, (ULONG_PTR)lpPerHandleData, 0) == NULL)
			{
				std::cout << Util::U2G("listenSocket绑定到io完成端口失败！错误代码：") << GetLastError() << std::endl;
				throw std::bad_exception();
			}
			//获取AcceptEx函数指针
			DWORD dwBytesRecvd = 0;
			GUID guidAcceptEx = WSAID_ACCEPTEX;
			if (WSAIoctl(_listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx), &_lpfnAcceptEx, sizeof(_lpfnAcceptEx), &dwBytesRecvd, NULL, NULL) != 0)	  //通过WSAIoctl获取AcceptEx函数指针时，只需要随便传递给WSAIoctl()一个有效的SOCKET即可，该Socket的类型不会影响获取的AcceptEx函数指针
			{
				std::cout << Util::U2G("获取AcceptEx函数指针失败！错误代码：") << WSAGetLastError() << std::endl;
				throw std::bad_exception();
			}
			//创建自动投递事件
			_postAcceptEvent = CreateEvent(NULL, false, true, NULL);//初始有信号自动复位事件
			//初始化IoDataPool
			_freeIoDataPool.FillPool(_freeIoDataPool.FillSize);
		}

		IOCPServer::~IOCPServer()
		{
			// TODO: 清理资源
		}


		void IOCPServer::Start()
		{
			//开启监听
			if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR)
			{
				std::cout << Util::U2G("listenSocket开启监听失败！错误代码：") << WSAGetLastError() << std::endl;
				throw std::bad_exception();
			}
			//创建工作线程
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			for (size_t i = 0; i < sysInfo.dwNumberOfProcessors; i++)
			{
				HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, WorksThread, this, 0, NULL);
				if (thread == NULL)
				{
					std::cout << Util::U2G("创建工作线程失败！错误代码：") << GetLastError() << std::endl;
					throw std::bad_exception();
				}
				CloseHandle(thread);
			}
			std::cout << Util::U2G("服务器已启动！等待客户端连接！") << std::endl;
			//动态投递acceptEx
			while (true)
			{
				WaitForSingleObject(_postAcceptEvent, INFINITE);
				for (size_t i = 0; i < _postAcceptCount; i++)
				{
					PostAccept();
				}
			}
		}


		bool IOCPServer::Stop()
		{
			// TODO: 关闭服务器并清理资源
			return false;
		}

		bool IOCPServer::PostAccept()
		{
			DWORD dwBytesRecvd = 0;
			PerIoData* lpPerIoData = _freeIoDataPool.Pop();
			lpPerIoData->operatorType = OperatorType::ACCEPT;
			lpPerIoData->acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			//投递acceptEx
			if (_lpfnAcceptEx(_listenSocket, lpPerIoData->acceptSocket, &lpPerIoData->buffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytesRecvd, &lpPerIoData->overlapped) == NULL)
			{
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					std::cout << Util::U2G("投递acceptEx失败！错误代码：") << WSAGetLastError() << std::endl;
					//清理资源
					closesocket(lpPerIoData->acceptSocket);
					_freeIoDataPool.Push(lpPerIoData);
					return false;
				}
			}
			_freeAcceptSockCount++;//atomic类型，无需手动加锁

			return true;
		}

		bool IOCPServer::PostRecv(SOCKET socket)
		{
			DWORD dwFlags = 0;
			PerIoData* lpPerIoData = _freeIoDataPool.Pop();
			lpPerIoData->operatorType = OperatorType::RECV;         //将状态设置成接收
			if (WSARecv(socket, &lpPerIoData->wsabuf, 1, NULL, &dwFlags, &lpPerIoData->overlapped, NULL) == SOCKET_ERROR)
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					std::cout << Util::U2G("WSARecv失败！错误代码：") << WSAGetLastError() << std::endl;
					_freeIoDataPool.Push(lpPerIoData);
					return false;
				}
			}
			return true;
		}

		bool IOCPServer::PostSend(SOCKET socket, std::string msg)
		{
			DWORD dwFlags = 0;
			PerIoData* lpPerIoData = _freeIoDataPool.Pop();
			lpPerIoData->operatorType = OperatorType::SEND;         //将状态设置成发送
			int msgLen = msg.length();
			///////////////////////////////
			std::cout << "send:" << Util::U2G(msg.c_str()) << std::endl;
			memcpy_s(lpPerIoData->wsabuf.buf, _TRUNCATE, (byte*)& msgLen, 4);	//封装包头
			strncpy_s(lpPerIoData->wsabuf.buf + 4, _TRUNCATE, msg.c_str(), msgLen);	//封装数据
			lpPerIoData->wsabuf.len = msgLen + 4;
			//memcpy_s(lpPerIoData->wsabuf.buf, _TRUNCATE, msg.c_str(), msgLen);
			//lpPerIoData->wsabuf.len = msgLen;
			if (WSASend(socket, &lpPerIoData->wsabuf, 1, NULL, dwFlags, &lpPerIoData->overlapped, NULL) == SOCKET_ERROR)
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					std::cout << Util::U2G("WSASend失败！错误代码：") << WSAGetLastError() << std::endl;
					_freeIoDataPool.Push(lpPerIoData);
					return false;
				}
			}

			return true;
		}

		void IOCPServer::ProcessAccept(PerHandleData* lpListenSockHandleData, PerIoData* lpPerIoData)
		{
			//未连接的acceptSocket减少
			_freeAcceptSockCount--;
			if (_freeAcceptSockCount <= 0)
			{
				SetEvent(_postAcceptEvent);	//投递acceptEx
			}
			//When the AcceptEx function returns, the socket sAcceptSocket is in the default state for a connected socket. The socket sAcceptSocket does not inherit the properties of the socket associated with sListenSocket parameter until SO_UPDATE_ACCEPT_CONTEXT is set on the socket.
			if (setsockopt(lpPerIoData->acceptSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*) & (lpListenSockHandleData->socket), sizeof(lpListenSockHandleData->socket)) == SOCKET_ERROR)
			{
				std::cout << Util::U2G("setsockopt失败！错误代码：") << GetLastError() << std::endl;
				//清空lpPerIoData数据
				_freeIoDataPool.Push(lpPerIoData);
				return;
			}
			//创建acceptSocket的PerHandleData
			PerHandleData* lpPerHandleData = new PerHandleData();
			lpPerHandleData->socket = lpPerIoData->acceptSocket;
			//lpPerHandleData->sendMessageFn = [this, lpPerHandleData](std::string msg) { PostSend(lpPerHandleData->socket, msg); };	//注册回调
			lpPerHandleData->sendMessageFn = std::bind(&IOCPServer::PostSend, this, lpPerHandleData->socket, std::placeholders::_1);
			//获取客户端地址
			int clientAddrLen = sizeof(SOCKADDR_IN);
			if (getpeername(lpPerHandleData->socket, (SOCKADDR*)& lpPerHandleData->clientAddr, &clientAddrLen) == SOCKET_ERROR)
			{
				std::cout << Util::U2G("getpeername失败！错误代码：") << GetLastError() << std::endl;
				//清空lpPerIoData数据
				_freeIoDataPool.Push(lpPerIoData);
				return;
			}
			//线程互斥执行
			{
				std::lock_guard<std::mutex> lock(_threadMtx);	//RAII
				std::cout << inet_ntoa(lpPerHandleData->clientAddr.sin_addr) << ":" << ntohs(lpPerHandleData->clientAddr.sin_port) << Util::U2G(" 成功连接!") << std::endl;	  //使用inet_ntoa要关闭SDL检查
			}
			//将acceptSocket与完成端口绑定
			if (CreateIoCompletionPort((HANDLE)lpPerHandleData->socket, _completionPort, (ULONG_PTR)lpPerHandleData, 0) == NULL)
			{
				std::cout << Util::U2G("acceptSocket绑定到io完成端口失败！错误代码：") << GetLastError() << std::endl;
				//清空lpPerIoData数据
				_freeIoDataPool.Push(lpPerIoData);
				return;
			}
			//清空lpPerIoData数据
			_freeIoDataPool.Push(lpPerIoData);
			//投递WASRecv
			PostRecv(lpPerHandleData->socket);
		}

		void IOCPServer::ProcessRecv(PerHandleData* lpPerHandleData, PerIoData* lpPerIoData, DWORD dwBytesTransferred)
		{
			//std::cout << Util::U2G("接收了") << dwBytesTransferred << Util::U2G("字节") << std::endl;
			lpPerHandleData->recvMsgQueue.Enqueue(lpPerIoData->buffer, dwBytesTransferred);
			//用线程池来分发消息,不阻塞消息接收
			_dispatchCmdThreadPool.Enqueue(&CommandDispatcher::StartDispatch, lpPerHandleData);

			//清空lpPerIoData数据
			_freeIoDataPool.Push(lpPerIoData);
			//继续投递WSARecv
			PostRecv(lpPerHandleData->socket);
		}

		void IOCPServer::ProcessSend(PerHandleData* lpPerHandleData, PerIoData* lpPerIoData, DWORD dwBytesTransferred)
		{
			//std::cout << Util::U2G("发送了") << dwBytesTransferred << Util::U2G("字节") << std::endl;
			//清空lpPerIoData数据
			_freeIoDataPool.Push(lpPerIoData);

		}

		unsigned int __stdcall IOCPServer::WorksThread(LPVOID lpParam)
		{
			DWORD dwBytesTransferred;
			IOCPServer* pThis = (IOCPServer*)lpParam;
			PerHandleData* lpPerHandleData;
			PerIoData* lpPerIoData;
			while (true)
			{
				lpPerHandleData = nullptr;
				lpPerIoData = nullptr;
				if (GetQueuedCompletionStatus(pThis->_completionPort, &dwBytesTransferred, (PULONG_PTR)& lpPerHandleData, (LPOVERLAPPED*)& lpPerIoData, INFINITE) == NULL)
				{
					std::cout << Util::U2G("GetQueuedCompletionStatus失败！错误代码：") << GetLastError() << std::endl;
					if (lpPerHandleData != nullptr)
					{
						closesocket(lpPerHandleData->socket);
						delete lpPerHandleData;
						lpPerHandleData = nullptr;
					}
					if (lpPerIoData != nullptr)
					{
						pThis->_freeIoDataPool.Push(lpPerIoData);
					}
					//socket要与完成端口解除绑定吗?网络异常这一块还不了解
					return -1;
				}
				//主动判断连接是否断开
				if (dwBytesTransferred == 0 && (lpPerIoData->operatorType == OperatorType::RECV || lpPerIoData->operatorType == OperatorType::SEND))
				{
					std::cout << inet_ntoa(lpPerHandleData->clientAddr.sin_addr) << ":" << ntohs(lpPerHandleData->clientAddr.sin_port) << Util::U2G(" 断开连接!") << std::endl;
					// TODO: 发送断开命令给业务逻辑进行处理

					closesocket(lpPerHandleData->socket);
					delete lpPerHandleData;
					lpPerHandleData = nullptr;
					pThis->_freeIoDataPool.Push(lpPerIoData);
				}

				switch (lpPerIoData->operatorType)
				{
				case OperatorType::ACCEPT:
					pThis->ProcessAccept(lpPerHandleData, lpPerIoData);
					break;
				case OperatorType::RECV:
					pThis->ProcessRecv(lpPerHandleData, lpPerIoData, dwBytesTransferred);
					break;
				case OperatorType::SEND:
					pThis->ProcessSend(lpPerHandleData, lpPerIoData, dwBytesTransferred);
					break;
				default:
					break;
				}

			}
			return 0;
		}

		FreeIoDataPool::~FreeIoDataPool()
		{
			ReducePool(GetCount());
		}

		void FreeIoDataPool::Push(PerIoData* lpPerIoData)
		{
			if (lpPerIoData == nullptr) return;
			//初始化PerIoData
			ZeroMemory(&(lpPerIoData->overlapped), sizeof(OVERLAPPED));
			ZeroMemory(&lpPerIoData->buffer, sizeof(DATA_BUFSIZE));
			lpPerIoData->wsabuf.len = DATA_BUFSIZE;
			lpPerIoData->operatorType = OperatorType::NONE;
			lpPerIoData->acceptSocket = NULL;

			std::lock_guard<std::mutex> lock(_ioDataPoolMtx);	//构造时自动上锁，析构时自动解锁
			_pool.push(lpPerIoData);
		}

		PerIoData* FreeIoDataPool::Pop()
		{
			if (_pool.size() <= 0)
			{
				FillPool(FillSize);
			}
			std::lock_guard<std::mutex> lock(_ioDataPoolMtx);
			PerIoData* lpPerIoData = _pool.top();
			_pool.pop();

			return lpPerIoData;
		}

		void FreeIoDataPool::FillPool(int count)
		{
			PerIoData* lpPerIoData;
			std::lock_guard<std::mutex> lock(_ioDataPoolMtx);
			for (size_t i = 0; i < count; i++)
			{
				//初始化PerIoData
				lpPerIoData = new PerIoData();
				ZeroMemory(&(lpPerIoData->overlapped), sizeof(OVERLAPPED));
				lpPerIoData->wsabuf.buf = lpPerIoData->buffer;
				lpPerIoData->wsabuf.len = DATA_BUFSIZE;

				_pool.push(lpPerIoData);
			}
		}

		void FreeIoDataPool::ReducePool(int count)
		{
			if (count > GetCount())
			{
				count = GetCount();
			}
			for (size_t i = 0; i < count; i++)
			{
				delete Pop();
			}
		}

		void RecvBufQueue::Enqueue(char* buffer, int size)
		{
			if (_rear + size > _size)
			{
				std::cout << Util::U2G("BufferQueue大小不足!") << std::endl;
				throw std::out_of_range("");
			}
			std::lock_guard<std::mutex> lock(_recvBufQueueMtx);
			memcpy_s(_data + _rear, _TRUNCATE, buffer, size);
			_rear += size;
		}

		std::string RecvBufQueue::Dequeue()
		{
			if (_rear > 4)
			{
				std::lock_guard<std::mutex> lck(_recvBufQueueMtx);
				char packHead[4];
				memcpy_s(packHead, 4, _data, 4);	//获取包大小
				int packLen = *(int*)packHead;	//将char数组转换为int
				if (_rear < packLen + 4)	//残包,不需要出列，等待下次接收
				{
					return "";
				}
				else	//处理粘包
				{
					char DetachedMsg[DATA_BUFSIZE];
					strncpy_s(DetachedMsg, sizeof(DetachedMsg), _data + 4, packLen);	//取出单条消息
					DetachedMsg[packLen] = 0;	//添加结尾符
					memmove_s(_data, sizeof(_data), _data + 4 + packLen, _rear - (4 + packLen));	//移动数据,使队首始终在0下标处
					_rear -= (4 + packLen);	//修改_rear
					return std::move(DetachedMsg);	//用移动语义,不然会被析构掉
				}
			}
			else if (_rear >= 0)	//数据在包头处断开或正好处理完,不需要出列，等待下次接收
			{
				return "";
			}
			else	//_rear小于0,出现错误
			{
				std::cout << Util::U2G("_rear小于0!") << std::endl;
				throw std::bad_exception();
			}
		}
	}
}