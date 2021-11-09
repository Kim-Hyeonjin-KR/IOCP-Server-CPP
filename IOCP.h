#pragma once

#include"global.h"
#include"SOC.h"

//�ٸ� �ҽ��ڵ��� ���������� ����ϴ� ���. �ʿ�� ����.
//class IOCP;
//extern IOCP* m_LoginManager;

class IOCP
{
private:
	HANDLE				m_hcp;		// �̰� �ڵ� ���� �����Ͷ� h c p �ΰ���?
									// �̰� IOCP�� �ڵ����� ����¿Ϸ���Ʈ.
	std::list<HANDLE>	m_hThread_list;	// ������ ���� ���ͷ� �����ؼ� ��� �����尡 �������Ǹ� ���ξ����� ������ �� �ֵ���
	void				Running();
	//bool			Init_IOCPPort();


public:


	static DWORD __stdcall	WorkerThread(void* _ptr);
	
	bool			Create_IOCPPort();		//Init���� Create�� ��Ī ����
	bool			End_IOCPPort();			//�ʿ��ϴٸ� ���� �����Ұ�
	bool		RegisterIocp(SOCKET);	//**���� �޼���� ��ȯ
	bool		PostQueuedNewAccpetSock(WSAOVERLAPPED_EX*);	//**���� �߰��� �Լ�

	HANDLE GetHandle();


	virtual void	accepted(void* _socptr)	= 0;
	virtual void	recved(void* object, int bytes)		= 0;
	virtual void	sent(void* object, int bytes)		= 0;
	virtual void	disconnect(void* _socptr)	= 0;
};