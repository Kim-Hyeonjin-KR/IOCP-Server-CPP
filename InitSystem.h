#pragma once

#include "global.h"
#include "IOCP.h"

//class IOCP;

// std::unique_ptr<NetworkManager> NetworkManager::sInstance; ??

// ������ ���������µ� ����� ���� �޴� ����. TCPSocket�� �Լ����� ����� ��ü ���������� ����ϱ� ���� protect�� ��������. �ϴ� �� ������ �����ϸ鼭 ���� ��Ű�� ���ؼ� �ӽ���ġ.
class InitSystem : public TCPSocket, public TemplateSingleton<InitSystem>
{
private:
	//SOCKET	    m_ListenSock;
	//TCPSocket*	m_ListenTCPSock;		// SOCKET�� ��� ���� �ʰ� ���� Ŭ������ ��ü�� ��� �ִ� ������.
											// 1. SOCKET�� ����� ������ TCPSocket�� ���� ���� �Լ��� ����Ѵٰ� ġ�� �׳� ����ϰ� TCPSocket�� ������ ����.
											// 2. SOCKET�� ����� ������ ���� �������� TCPSocket�� ��ɵ鿡 �����Ѵٰ� ġ�� CreateAcceptSock �Լ����� �����ڱ��� ����� �ȴ�.
	// �� �� �ʿ� ����. ��ó�� �θ�� ����Ǿ� �־ TCPSocket ��ü�� ���� �ȸ��� ��� �������� �ڵ����� ������ �ȴ�.
	// �׷��� TCPSocket�� ������ �ʾƵ� �ȴ�.

public:
	bool Create();

	//���� ��Ʈ�� ���� �����
	bool Init(u_short _inPort = SERVERPORT);
	//static DWORD __stdcall	AcceptThread(void* _ptr);
	//DWORD __stdcall  AcceptThread(void* arg);

	bool Run();
};