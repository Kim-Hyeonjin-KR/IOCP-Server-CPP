#include "InitSystem.h"
#include "LoginManager.h"
#include "SOC.h"
	
// 클래스 전방선언
//class TCPSocket;

class IOCP;


bool InitSystem::Create()
{
	return true;
}

bool InitSystem::Init(u_short _serverport)
{
	// WinSock초기화

	//TCPSocket::WinSockInit();
	
	// iocp 입출력 포트 생성 -iocp
	// IOCP::Create_IOCPPort();
	// void LoginManager::Create()에서 처리하도록 뺐음.
	
	// AcceptSock 생성 -메인
	//TCPSocket::CreateAcceptSock();
	// TCPSocket을 상속받는 과정에서 이미 생성자에 포함되어 있는 내용이므로 건너뜀.

	// binding -메인
	TCPSocket::Bind();
	
	// debug
	printf("초기화 시스템 리슨 대기상태 진입 완료. port : %d", _serverport);
	
	// listening
	TCPSocket::Listen();
	


	// create accept thread
	//HANDLE hThread = CreateThread(NULL, 0, AcceptThread, NULL , 0, NULL); // 만약 InitSystem자체가 서버의 리슨소캣을 관리한다면. 굳이 매개변수로 안넘겨줘도 될듯 하다.
	//											// AcceptThread 왜 못쓰는지 이유 배웠던거 같은데.. 모르겠다.
	//											// 쓰레드가 static이 아니라서 사용하지 못했었다. 쓰레드는 꼭 static으로 만들어서 쓰자

	return true;
}

bool InitSystem::Run()
{
	//arg가 뭘까
	//리슨소캣 헨들


	//HANDLE hcp = (HANDLE)arg;
	//arg로 들어온 리슨소캣 핸들에 접근하여 arg->Accept();

	//** 상속받았기 때문에 따로 arg로 리슨소캣을 받지 않고도 Accept할 수 있다. 그러니 필요 없을것이다.
	//** 다만 InitSystem이 SOC을 상속받지 않고도 SOC의 기능들을 쓰는 방법은 아직 잘 모르겠다.

	// accept()
	TCPSocket* client_sock = TCPSocket::Accept();



	// 소켓과 입출력 완료 포트 연결
	// iocp의 주소, 클라의 소켓
	//bool result = _iocp->RegisterIocp(client_sock->m_sock);	// **m_sock 접근권한 때문에 퍼블릭으로 일단 변경
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
	printf("PostQueuedCompletionStatus 실패\n");

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
//	//arg로 들어온 리슨소캣 핸들에 접근하여 arg->Accept();
//
//	// accept()
//	TCPSocket* client_sock = m_server_soc->Accept();
//	//TCPSocket* client_sock = TCPSocket::Accept();
//	//TCPSocket* client_sock = 서버소켓->Accept();
//	
//	// 소켓과 입출력 완료 포트 연결
//	m_IOCP->RegisterIocp(client_sock->m_sock);
//
//
//
//
//	LoginManager::GetInstance().Init();
//}
