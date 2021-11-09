#pragma once

#include"global.h"
#include"SOC.h"

//다른 소스코드의 전역변수를 사용하는 방법. 필요는 없음.
//class IOCP;
//extern IOCP* m_LoginManager;

class IOCP
{
private:
	HANDLE				m_hcp;		// 이게 핸들 소켓 포인터라서 h c p 인가요?
									// 이건 IOCP의 핸들이자 입출력완료포트.
	std::list<HANDLE>	m_hThread_list;	// 쓰레드 헨들들 백터로 관리해서 모든 쓰레드가 사용종료되면 메인쓰레드 종료할 수 있도록
	void				Running();
	//bool			Init_IOCPPort();


public:


	static DWORD __stdcall	WorkerThread(void* _ptr);
	
	bool			Create_IOCPPort();		//Init에서 Create로 명칭 변경
	bool			End_IOCPPort();			//필요하다면 내용 구현할것
	bool		RegisterIocp(SOCKET);	//**정적 메서드로 변환
	bool		PostQueuedNewAccpetSock(WSAOVERLAPPED_EX*);	//**새로 추가한 함수

	HANDLE GetHandle();


	virtual void	accepted(void* _socptr)	= 0;
	virtual void	recved(void* object, int bytes)		= 0;
	virtual void	sent(void* object, int bytes)		= 0;
	virtual void	disconnect(void* _socptr)	= 0;
};