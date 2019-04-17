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
			//��ʼ��socket
			WORD versionRequest = MAKEWORD(2, 2);
			WSADATA wsaData;
			if (WSAStartup(versionRequest, &wsaData) != 0)	//����ֵΪ0����ɹ� 
			{
				std::cout << Util::U2G("��ʼ��socketʧ�ܣ�������룺") << WSAGetLastError() << std::endl;
				throw std::bad_exception();
			}
			//��ʼ����ַ
			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.S_un.S_addr = INADDR_ANY;
			addr.sin_port = htons(port);
			//��������socket
			_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (_listenSocket == INVALID_SOCKET)
			{
				std::cout << Util::U2G("listenSocket����ʧ�ܣ�������룺") << WSAGetLastError() << std::endl;
				throw std::bad_exception();
			}
			//�󶨼���socket
			if (bind(_listenSocket, (sockaddr*)& addr, sizeof(addr)) == SOCKET_ERROR)
			{
				std::cout << Util::U2G("��listenSocketʧ�ܣ�������룺") << WSAGetLastError() << std::endl;
				throw std::bad_exception();
			}
			//����io��ɶ˿�
			_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
			if (_completionPort == NULL)
			{
				std::cout << Util::U2G("����completionPortʧ�ܣ�������룺") << GetLastError() << std::endl;
				throw std::bad_exception();
			}
			//������socket�󶨵�io��ɶ˿�
			PerHandleData* lpPerHandleData = new PerHandleData();
			lpPerHandleData->socket = _listenSocket;
			if (CreateIoCompletionPort((HANDLE)_listenSocket, _completionPort, (ULONG_PTR)lpPerHandleData, 0) == NULL)
			{
				std::cout << Util::U2G("listenSocket�󶨵�io��ɶ˿�ʧ�ܣ�������룺") << GetLastError() << std::endl;
				throw std::bad_exception();
			}
			//��ȡAcceptEx����ָ��
			DWORD dwBytesRecvd = 0;
			GUID guidAcceptEx = WSAID_ACCEPTEX;
			if (WSAIoctl(_listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx), &_lpfnAcceptEx, sizeof(_lpfnAcceptEx), &dwBytesRecvd, NULL, NULL) != 0)	  //ͨ��WSAIoctl��ȡAcceptEx����ָ��ʱ��ֻ��Ҫ��㴫�ݸ�WSAIoctl()һ����Ч��SOCKET���ɣ���Socket�����Ͳ���Ӱ���ȡ��AcceptEx����ָ��
			{
				std::cout << Util::U2G("��ȡAcceptEx����ָ��ʧ�ܣ�������룺") << WSAGetLastError() << std::endl;
				throw std::bad_exception();
			}
			//�����Զ�Ͷ���¼�
			_postAcceptEvent = CreateEvent(NULL, false, true, NULL);//��ʼ���ź��Զ���λ�¼�
			//��ʼ��IoDataPool
			_freeIoDataPool.FillPool(_freeIoDataPool.FillSize);
		}

		IOCPServer::~IOCPServer()
		{
			// TODO: ������Դ
		}


		void IOCPServer::Start()
		{
			//��������
			if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR)
			{
				std::cout << Util::U2G("listenSocket��������ʧ�ܣ�������룺") << WSAGetLastError() << std::endl;
				throw std::bad_exception();
			}
			//���������߳�
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			for (size_t i = 0; i < sysInfo.dwNumberOfProcessors; i++)
			{
				HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, WorksThread, this, 0, NULL);
				if (thread == NULL)
				{
					std::cout << Util::U2G("���������߳�ʧ�ܣ�������룺") << GetLastError() << std::endl;
					throw std::bad_exception();
				}
				CloseHandle(thread);
			}
			std::cout << Util::U2G("���������������ȴ��ͻ������ӣ�") << std::endl;
			//��̬Ͷ��acceptEx
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
			// TODO: �رշ�������������Դ
			return false;
		}

		bool IOCPServer::PostAccept()
		{
			DWORD dwBytesRecvd = 0;
			PerIoData* lpPerIoData = _freeIoDataPool.Pop();
			lpPerIoData->operatorType = OperatorType::ACCEPT;
			lpPerIoData->acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			//Ͷ��acceptEx
			if (_lpfnAcceptEx(_listenSocket, lpPerIoData->acceptSocket, &lpPerIoData->buffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytesRecvd, &lpPerIoData->overlapped) == NULL)
			{
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					std::cout << Util::U2G("Ͷ��acceptExʧ�ܣ�������룺") << WSAGetLastError() << std::endl;
					//������Դ
					closesocket(lpPerIoData->acceptSocket);
					_freeIoDataPool.Push(lpPerIoData);
					return false;
				}
			}
			_freeAcceptSockCount++;//atomic���ͣ������ֶ�����

			return true;
		}

		bool IOCPServer::PostRecv(SOCKET socket)
		{
			DWORD dwFlags = 0;
			PerIoData* lpPerIoData = _freeIoDataPool.Pop();
			lpPerIoData->operatorType = OperatorType::RECV;         //��״̬���óɽ���
			if (WSARecv(socket, &lpPerIoData->wsabuf, 1, NULL, &dwFlags, &lpPerIoData->overlapped, NULL) == SOCKET_ERROR)
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					std::cout << Util::U2G("WSARecvʧ�ܣ�������룺") << WSAGetLastError() << std::endl;
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
			lpPerIoData->operatorType = OperatorType::SEND;         //��״̬���óɷ���
			int msgLen = msg.length();
			///////////////////////////////
			std::cout << "send:" << Util::U2G(msg.c_str()) << std::endl;
			memcpy_s(lpPerIoData->wsabuf.buf, _TRUNCATE, (byte*)& msgLen, 4);	//��װ��ͷ
			strncpy_s(lpPerIoData->wsabuf.buf + 4, _TRUNCATE, msg.c_str(), msgLen);	//��װ����
			lpPerIoData->wsabuf.len = msgLen + 4;
			//memcpy_s(lpPerIoData->wsabuf.buf, _TRUNCATE, msg.c_str(), msgLen);
			//lpPerIoData->wsabuf.len = msgLen;
			if (WSASend(socket, &lpPerIoData->wsabuf, 1, NULL, dwFlags, &lpPerIoData->overlapped, NULL) == SOCKET_ERROR)
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					std::cout << Util::U2G("WSASendʧ�ܣ�������룺") << WSAGetLastError() << std::endl;
					_freeIoDataPool.Push(lpPerIoData);
					return false;
				}
			}

			return true;
		}

		void IOCPServer::ProcessAccept(PerHandleData* lpListenSockHandleData, PerIoData* lpPerIoData)
		{
			//δ���ӵ�acceptSocket����
			_freeAcceptSockCount--;
			if (_freeAcceptSockCount <= 0)
			{
				SetEvent(_postAcceptEvent);	//Ͷ��acceptEx
			}
			//When the AcceptEx function returns, the socket sAcceptSocket is in the default state for a connected socket. The socket sAcceptSocket does not inherit the properties of the socket associated with sListenSocket parameter until SO_UPDATE_ACCEPT_CONTEXT is set on the socket.
			if (setsockopt(lpPerIoData->acceptSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*) & (lpListenSockHandleData->socket), sizeof(lpListenSockHandleData->socket)) == SOCKET_ERROR)
			{
				std::cout << Util::U2G("setsockoptʧ�ܣ�������룺") << GetLastError() << std::endl;
				//���lpPerIoData����
				_freeIoDataPool.Push(lpPerIoData);
				return;
			}
			//����acceptSocket��PerHandleData
			PerHandleData* lpPerHandleData = new PerHandleData();
			lpPerHandleData->socket = lpPerIoData->acceptSocket;
			//lpPerHandleData->sendMessageFn = [this, lpPerHandleData](std::string msg) { PostSend(lpPerHandleData->socket, msg); };	//ע��ص�
			lpPerHandleData->sendMessageFn = std::bind(&IOCPServer::PostSend, this, lpPerHandleData->socket, std::placeholders::_1);
			//��ȡ�ͻ��˵�ַ
			int clientAddrLen = sizeof(SOCKADDR_IN);
			if (getpeername(lpPerHandleData->socket, (SOCKADDR*)& lpPerHandleData->clientAddr, &clientAddrLen) == SOCKET_ERROR)
			{
				std::cout << Util::U2G("getpeernameʧ�ܣ�������룺") << GetLastError() << std::endl;
				//���lpPerIoData����
				_freeIoDataPool.Push(lpPerIoData);
				return;
			}
			//�̻߳���ִ��
			{
				std::lock_guard<std::mutex> lock(_threadMtx);	//RAII
				std::cout << inet_ntoa(lpPerHandleData->clientAddr.sin_addr) << ":" << ntohs(lpPerHandleData->clientAddr.sin_port) << Util::U2G(" �ɹ�����!") << std::endl;	  //ʹ��inet_ntoaҪ�ر�SDL���
			}
			//��acceptSocket����ɶ˿ڰ�
			if (CreateIoCompletionPort((HANDLE)lpPerHandleData->socket, _completionPort, (ULONG_PTR)lpPerHandleData, 0) == NULL)
			{
				std::cout << Util::U2G("acceptSocket�󶨵�io��ɶ˿�ʧ�ܣ�������룺") << GetLastError() << std::endl;
				//���lpPerIoData����
				_freeIoDataPool.Push(lpPerIoData);
				return;
			}
			//���lpPerIoData����
			_freeIoDataPool.Push(lpPerIoData);
			//Ͷ��WASRecv
			PostRecv(lpPerHandleData->socket);
		}

		void IOCPServer::ProcessRecv(PerHandleData* lpPerHandleData, PerIoData* lpPerIoData, DWORD dwBytesTransferred)
		{
			//std::cout << Util::U2G("������") << dwBytesTransferred << Util::U2G("�ֽ�") << std::endl;
			lpPerHandleData->recvMsgQueue.Enqueue(lpPerIoData->buffer, dwBytesTransferred);
			//���̳߳����ַ���Ϣ,��������Ϣ����
			_dispatchCmdThreadPool.Enqueue(&CommandDispatcher::StartDispatch, lpPerHandleData);

			//���lpPerIoData����
			_freeIoDataPool.Push(lpPerIoData);
			//����Ͷ��WSARecv
			PostRecv(lpPerHandleData->socket);
		}

		void IOCPServer::ProcessSend(PerHandleData* lpPerHandleData, PerIoData* lpPerIoData, DWORD dwBytesTransferred)
		{
			//std::cout << Util::U2G("������") << dwBytesTransferred << Util::U2G("�ֽ�") << std::endl;
			//���lpPerIoData����
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
					std::cout << Util::U2G("GetQueuedCompletionStatusʧ�ܣ�������룺") << GetLastError() << std::endl;
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
					//socketҪ����ɶ˿ڽ������?�����쳣��һ�黹���˽�
					return -1;
				}
				//�����ж������Ƿ�Ͽ�
				if (dwBytesTransferred == 0 && (lpPerIoData->operatorType == OperatorType::RECV || lpPerIoData->operatorType == OperatorType::SEND))
				{
					std::cout << inet_ntoa(lpPerHandleData->clientAddr.sin_addr) << ":" << ntohs(lpPerHandleData->clientAddr.sin_port) << Util::U2G(" �Ͽ�����!") << std::endl;
					// TODO: ���ͶϿ������ҵ���߼����д���

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
			//��ʼ��PerIoData
			ZeroMemory(&(lpPerIoData->overlapped), sizeof(OVERLAPPED));
			ZeroMemory(&lpPerIoData->buffer, sizeof(DATA_BUFSIZE));
			lpPerIoData->wsabuf.len = DATA_BUFSIZE;
			lpPerIoData->operatorType = OperatorType::NONE;
			lpPerIoData->acceptSocket = NULL;

			std::lock_guard<std::mutex> lock(_ioDataPoolMtx);	//����ʱ�Զ�����������ʱ�Զ�����
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
				//��ʼ��PerIoData
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
				std::cout << Util::U2G("BufferQueue��С����!") << std::endl;
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
				memcpy_s(packHead, 4, _data, 4);	//��ȡ����С
				int packLen = *(int*)packHead;	//��char����ת��Ϊint
				if (_rear < packLen + 4)	//�а�,����Ҫ���У��ȴ��´ν���
				{
					return "";
				}
				else	//����ճ��
				{
					char DetachedMsg[DATA_BUFSIZE];
					strncpy_s(DetachedMsg, sizeof(DetachedMsg), _data + 4, packLen);	//ȡ��������Ϣ
					DetachedMsg[packLen] = 0;	//��ӽ�β��
					memmove_s(_data, sizeof(_data), _data + 4 + packLen, _rear - (4 + packLen));	//�ƶ�����,ʹ����ʼ����0�±괦
					_rear -= (4 + packLen);	//�޸�_rear
					return std::move(DetachedMsg);	//���ƶ�����,��Ȼ�ᱻ������
				}
			}
			else if (_rear >= 0)	//�����ڰ�ͷ���Ͽ������ô�����,����Ҫ���У��ȴ��´ν���
			{
				return "";
			}
			else	//_rearС��0,���ִ���
			{
				std::cout << Util::U2G("_rearС��0!") << std::endl;
				throw std::bad_exception();
			}
		}
	}
}