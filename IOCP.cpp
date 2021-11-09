#include "IOCP.h"

// https://sanghun219.tistory.com/104
// IOCP는 하나의 큐 인가? OS랑 연결되어 있고. 포트가 있어서 서버 소켓에 포트와 클라 소켓의 포트와 연결
// 되어 있는 건가?

bool IOCP::Create_IOCPPort()
{
	// 입출력 완료 포트 생성
	m_hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_hcp == NULL) return false;

	// CPU 개수 확인
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// (CPU 개수 * 2)개의 작업자 스레드 생성
	HANDLE hThread;
	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; i++) {
		//CreateThread	리턴값	 : 커널() 오브젝트의 핸들
		//		"		매개변수 : 1 보안 관련 설정. 디폴트 보안을 위해 NULL
		//						 : 2 쓰레드 생성 시 요구되는 스택의 크기. 0이면 디폴트 상태인 스택이 크기를 할당받음.
		//						 : 3 쓰레드로 돌릴 함수의 포인터
		//						 : 4 3으로 돌릴 함수에 전달할 매개변수
		//						 : 5 쓰레드 생성 이후 즉시 실행 가능 or 대기 상태 지정. 0이면 바로 실행 가능
		//						 : 6 쓰레드 생성 시 쓰레드의 ID. 이를 저장하기 위한 별도의 변수 포인터.
		//						 : 의문. 6번의 의미는 return되는 핸들 값과 값과 의미가 다른가?
		hThread = CreateThread(NULL, 0, WorkerThread, this, 0, NULL);
		if (hThread == NULL) return false;
		//CloseHandle(hThread); // 모든 쓰레드 끝나면 이 작업 해주기
		//핸들 벡터에 넣어주기
		m_hThread_list.push_back(hThread);
	}
	return true;
}

DWORD __stdcall IOCP::WorkerThread(void* _ptr)
{
	static_cast<IOCP*>(_ptr)->Running();
	return 0;
}


//사이즈, 구분 번호, 프로토콜, 패킹된 데이터
// 									= 데이터 : 사이즈, 데이터 프로토콜, 결과, 실제 데이터
//순으로 패킹되어 있는데. 매니저 프로토콜을 어디서 구분하여야 하는가.
//OverlappedEX에 포함되어 있는 void* ptr을 참조하여 구분하는것이 맞는가.
//맞다면 아래 Running()함수에서 해당 프로토콜을 찾아서 구분하는것이 맞는가.

void IOCP::Running()
{
	//dword, long*, 오버레피드ex만들고 오버레피드로캐스팅, 인피니트
	DWORD data_size;
	unsigned long ptr;
	WSAOVERLAPPED_EX* overlapped_ex;

	while (true)
	{
		// I/O 완료포트에서 패킷을 가져오는 함수
												//완료포트의 헨들
												//입출력 완료 연산의 결과로 전송된 데이터 크기
												//파일 헨들과 관련된 완료 키값의 포인터
												//overlapped 포인터
												//완료 패킷을 기다려주는 시간
		bool retval = GetQueuedCompletionStatus(m_hcp, &data_size, &ptr, (LPOVERLAPPED*)&overlapped_ex, INFINITE);


		//오류검사
		if (retval == false)
		{
			disconnect(overlapped_ex->_ptr);
		}



		//GetQued의 4번째 매개변수에서 IO_TYPE으로 접근하여 알아낸 값으로 함수 호출.
		switch (overlapped_ex->type)
		{
		case E_IO_TYPE::IO_Recv :
			recved(overlapped_ex->_ptr, (int)data_size);
			break;

		case E_IO_TYPE::IO_Send :
			sent(overlapped_ex->_ptr, (int)data_size);
			break;

		case E_IO_TYPE::IO_Accept :
			accepted(overlapped_ex->_ptr); //loginsseion 객체  생성및 iocp 포트에 소켓핸들 등록
			break;

		default:
			break;
		}
	}
}


bool	IOCP::RegisterIocp(SOCKET _sock)
{
	HANDLE result_handle;
	// 새로 들어온 클라 소켓을 IOCP입출력 포트랑 연결해줍니다.
	result_handle = CreateIoCompletionPort((HANDLE)_sock, m_hcp, NULL, 0);	//CompletionKey 대신 overlapped_ex구조체를 사용하기 때문에 NULL값을 매개변수로 씁니다.
													//**InitSystem에서 호출하기 위해 메서드를 static으로 만들었기 때문에. m_hcp또한 static이여야 한다.
	if (result_handle == NULL)
		return false;


	return true;
}

bool	IOCP::PostQueuedNewAccpetSock(WSAOVERLAPPED_EX* _ptr)
{
	return PostQueuedCompletionStatus(m_hcp, 0, NULL, (LPOVERLAPPED)_ptr);
}

HANDLE IOCP::GetHandle()
{

	return m_hcp;
}

