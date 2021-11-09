// https://recipes4dev.tistory.com/153

#pragma once
#include"global.h"

struct WSAOVERLAPPED_EX
{
	WSAOVERLAPPED overlapped;
	void* _ptr;		// � Ŭ���̾�Ʈ�� ��û�ΰ� == HANDLE
	E_IO_TYPE type;	// �����ΰ� ���ú��ΰ�
};

// ���� ���� ������ ���� ����ü
struct Recv_Buffer
{	
	char buf[BUFSIZE + 1];
	int target_bytes;
	int comp_bytes;	
};

struct Send_Buffer
{
	char* buf;	//����� �����ͷ� �����Ҵ�
	int target_bytes;
	int comp_bytes;
};

class TCPSocket
{
private:
	bool			r_sizeflag;	//recv������ ���.
	//SOCKET m_socket;

public:
	SOCKET			m_sock;

protected:
	//SOCKET			m_sock;
	WSAOVERLAPPED_EX  m_recv_overlapped;
	WSAOVERLAPPED_EX  m_send_overlapped;
	WSAOVERLAPPED_EX  m_accept_overlapped;

	Recv_Buffer		m_Recv_buffer;
	std::queue<Send_Buffer*> m_Send_buffer;

	////����� ť (�����Ҵ�)
	//TCPSocket();
	//TCPSocket(SOCKET inSocket);
	//~TCPSocket();

	bool			WinSockInit();	//��ȸ�� �ȴٸ� Init������ �ѱ⵵�� ����

	bool			Bind();	//���� ��Ʈ�� ���ε�
	bool			Bind(sockaddr_in _portAddr);	//��Ʈ���� ���� ���� �ּ�(���� ���п�)
	bool			Listen();

	//recv
	bool			Recv();
	E_RESULT		CompleteRecv(int _completebyte);		// comp recv�Լ��� comp send�Լ� ��ȯ���� �ٸ� ��Ȳ. E_RESULT��ȯ�� ������ ���� �ʴ´ٸ� �����Ұ�.

	//send
	bool			Send(char* _sendData, int _inLen);
	E_RESULT		CompleteSend(int _completebyte);


	//accept
	void CreateAcceptSock();
	TCPSocket* Accept();	//���� ������ �ּ�(���ſ� ���� �ܺ� ��ſ�)
	//End()


	//�����ڿ� �Ҹ��ڸ� public���� �����ϴ�.
	//������ �ӽ� ��ó�� InitSystem���� TCPSocket�� ��ӹ�����. 
	//���� �и���ų ��� protected�� ����� �� ���� ������ �����ϴ�. 
public:
	//����� ť (�����Ҵ�)
	TCPSocket();
	TCPSocket(SOCKET inSocket);
	~TCPSocket();
};




//bool			Connect(sockaddr_in _serverAddr);	//Ŭ�󿡼� ����� �Լ�