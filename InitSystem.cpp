#include "InitSystem.h"
#include "LoginManager.h"
#include "SOC.h"
	
// Ŭ���� ���漱��
//class TCPSocket;

class IOCP;


bool InitSystem::Create()
{
	return true;
}

bool InitSystem::Init(u_short _serverport)
{
	// WinSock�ʱ�ȭ

	//TCPSocket::WinSockInit();
	
	// iocp ����� ��Ʈ ���� -iocp
	// IOCP::Create_IOCPPort();
	// void LoginManager::Create()���� ó���ϵ��� ����.
	
	// AcceptSock ���� -����
	//TCPSocket::CreateAcceptSock();
	// TCPSocket�� ��ӹ޴� �������� �̹� �����ڿ� ���ԵǾ� �ִ� �����̹Ƿ� �ǳʶ�.

	// binding -����
	TCPSocket::Bind();
	
	// debug
	printf("�ʱ�ȭ �ý��� ���� ������ ���� �Ϸ�. port : %d", _serverport);
	
	// listening
	TCPSocket::Listen();
	


	// create accept thread
	//HANDLE hThread = CreateThread(NULL, 0, AcceptThread, NULL , 0, NULL); // ���� InitSystem��ü�� ������ ������Ĺ�� �����Ѵٸ�. ���� �Ű������� �ȳѰ��൵ �ɵ� �ϴ�.
	//											// AcceptThread �� �������� ���� ������� ������.. �𸣰ڴ�.
	//											// �����尡 static�� �ƴ϶� ������� ���߾���. ������� �� static���� ���� ����

	return true;
}

bool InitSystem::Run()
{
	//arg�� ����
	//������Ĺ ���


	//HANDLE hcp = (HANDLE)arg;
	//arg�� ���� ������Ĺ �ڵ鿡 �����Ͽ� arg->Accept();

	//** ��ӹ޾ұ� ������ ���� arg�� ������Ĺ�� ���� �ʰ� Accept�� �� �ִ�. �׷��� �ʿ� �������̴�.
	//** �ٸ� InitSystem�� SOC�� ��ӹ��� �ʰ� SOC�� ��ɵ��� ���� ����� ���� �� �𸣰ڴ�.

	// accept()
	TCPSocket* client_sock = TCPSocket::Accept();



	// ���ϰ� ����� �Ϸ� ��Ʈ ����
	// iocp�� �ּ�, Ŭ���� ����
	//bool result = _iocp->RegisterIocp(client_sock->m_sock);	// **m_sock ���ٱ��� ������ �ۺ����� �ϴ� ����
	/*bool result = IOCP::RegisterIocp(client_sock->m_sock);
	if (!result)
	{
		return false;
	}*/



	WSAOVERLAPPED_EX* ptr = new WSAOVERLAPPED_EX;
	ZeroMemory(ptr, sizeof(WSAOVERLAPPED_EX));
	ptr->type = E_IO_TYPE::IO_Accept;
	ptr->_ptr = client_sock;
	int result = PostQueuedCompletionStatus(LoginManager::GetInstance()->GetHandle(), -1, NULL, (LPOVERLAPPED)ptr);

	if (!result)
	printf("PostQueuedCompletionStatus ����\n");

	//return result;
	return true;
	//LoginManager::GetInstance()->Run();
}


//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
//LPTHREAD_START_ROUTINE

// DWORD __stdcall InitSystem::AcceptThread(void* arg)
//{
//	int retval;
//	HANDLE hcp = (HANDLE)arg;
//	//arg�� ���� ������Ĺ �ڵ鿡 �����Ͽ� arg->Accept();
//
//	// accept()
//	TCPSocket* client_sock = m_server_soc->Accept();
//	//TCPSocket* client_sock = TCPSocket::Accept();
//	//TCPSocket* client_sock = ��������->Accept();
//	
//	// ���ϰ� ����� �Ϸ� ��Ʈ ����
//	m_IOCP->RegisterIocp(client_sock->m_sock);
//
//
//
//
//	LoginManager::GetInstance().Init();
//}
