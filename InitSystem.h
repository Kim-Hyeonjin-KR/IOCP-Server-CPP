#pragma once

#include "global.h"
#include "IOCP.h"

//class IOCP;

// std::unique_ptr<NetworkManager> NetworkManager::sInstance; ??

// 구조가 복잡해지는데 상속을 굳이 받는 이유. TCPSocket의 함수들의 기능을 객체 지향적으로 사용하기 위해 protect로 선언했음. 일단 이 형식을 유지하면서 빌드 시키기 위해서 임시조치.
class InitSystem : public TCPSocket, public TemplateSingleton<InitSystem>
{
private:
	//SOCKET	    m_ListenSock;
	//TCPSocket*	m_ListenTCPSock;		// SOCKET만 들고 있지 않고 굳이 클래스로 객체를 들고 있는 이유는.
											// 1. SOCKET의 헨들을 가지고 TCPSocket을 만들어서 안의 함수를 사용한다고 치면 그냥 깔끔하게 TCPSocket을 가지고 있자.
											// 2. SOCKET의 헨들을 가지고 참조 형식으로 TCPSocket의 기능들에 접근한다고 치면 CreateAcceptSock 함수부터 생성자까지 모순이 된다.
	// 둘 다 필요 없다. 어처피 부모랑 연결되어 있어서 TCPSocket 객체를 직접 안만들어도 상속 과정에서 자동으로 생성이 된다.
	// 그래서 TCPSocket을 가지지 않아도 된다.

public:
	bool Create();

	//서버 포트로 서버 만들기
	bool Init(u_short _inPort = SERVERPORT);
	//static DWORD __stdcall	AcceptThread(void* _ptr);
	//DWORD __stdcall  AcceptThread(void* arg);

	bool Run();
};