// https://recipes4dev.tistory.com/153

#pragma once
#include"global.h"

struct WSAOVERLAPPED_EX
{
	WSAOVERLAPPED overlapped;
	void* _ptr;		// 어떤 클라이언트의 요청인가 == HANDLE
	E_IO_TYPE type;	// 샌드인가 리시브인가
};

// 소켓 정보 저장을 위한 구조체
struct Recv_Buffer
{	
	char buf[BUFSIZE + 1];
	int target_bytes;
	int comp_bytes;	
};

struct Send_Buffer
{
	char* buf;	//샌드는 포인터로 동적할당
	int target_bytes;
	int comp_bytes;
};

class TCPSocket
{
private:
	bool			r_sizeflag;	//recv에서만 사용.
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

	////샌드용 큐 (동적할당)
	//TCPSocket();
	//TCPSocket(SOCKET inSocket);
	//~TCPSocket();

	bool			WinSockInit();	//기회가 된다면 Init쪽으로 넘기도록 하자

	bool			Bind();	//서버 포트랑 바인딩
	bool			Bind(sockaddr_in _portAddr);	//포트마다 사용될 내부 주소(내부 구분용)
	bool			Listen();

	//recv
	bool			Recv();
	E_RESULT		CompleteRecv(int _completebyte);		// comp recv함수랑 comp send함수 반환값이 다른 상황. E_RESULT반환이 문제가 되지 않는다면 통일할것.

	//send
	bool			Send(char* _sendData, int _inLen);
	E_RESULT		CompleteSend(int _completebyte);


	//accept
	void CreateAcceptSock();
	TCPSocket* Accept();	//서버 소켓의 주소(수신에 사용될 외부 통신용)
	//End()


	//생성자와 소멸자를 public으로 뺐습니다.
	//지금은 임시 대처로 InitSystem에서 TCPSocket을 상속받지만. 
	//추후 분리시킬 경우 protected로 사용할 수 없기 때문에 뺐습니다. 
public:
	//샌드용 큐 (동적할당)
	TCPSocket();
	TCPSocket(SOCKET inSocket);
	~TCPSocket();
};




//bool			Connect(sockaddr_in _serverAddr);	//클라에서 사용할 함수