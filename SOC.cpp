#include "SOC.h"

bool TCPSocket::WinSockInit()	//SockInit() -> WinSockInit() 으로 명칭 변경
{
	// [전체 흐름] 윈속 초기화 -> 소켓 생성 -> 통신 -> 소켓 닫기 -> 윈속 종료
	// 윈속 초기화.
	WSADATA wsa;

	// WSAStartup 사용할 윈도우 소켓의 버전을 요청하고 이에따른 윈도우 소켓 라이브러리를 초기화 해줍니다.
	// MAKEWORD(2,2), wsa는 잘 모르게따 
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;
	return true;
}

TCPSocket::TCPSocket()
{
	//AcceptSocket을 만든다는건. 처음으로 소켓을 생성한다는 뜻이니 WinSock초기화를 먼저 진행해줍니다. 
	WinSockInit();

	// 생성자 호출 시 매개변수가 없으면 CreateAcceptSocket으로 초기화 합니다.
	CreateAcceptSock();

	// overlapped_ex 구조체 초기화
	m_recv_overlapped.type		= E_IO_TYPE::IO_Recv;
	m_send_overlapped.type		= E_IO_TYPE::IO_Send;
	m_accept_overlapped.type	= E_IO_TYPE::IO_Accept;
}

TCPSocket::TCPSocket(SOCKET inSocket)
{
	m_sock = inSocket;

	// overlapped_ex 구조체 초기화
	m_recv_overlapped.type = E_IO_TYPE::IO_Recv;
	m_send_overlapped.type = E_IO_TYPE::IO_Send;
	m_accept_overlapped.type = E_IO_TYPE::IO_Accept;
}

TCPSocket::~TCPSocket()
{
	closesocket(m_sock);
}

bool TCPSocket::Bind()
{
	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;	//ipv4주소체계 사용
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	int retval = bind(m_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		err_quit("bind()");
		return false;
	}

	return true;
}

bool TCPSocket::Bind(sockaddr_in _portAddr)
{
	// bind()
	int retval = bind(m_sock, (SOCKADDR*)&_portAddr, sizeof(_portAddr));
	if (retval == SOCKET_ERROR)
	{
		err_quit("bind()");
		return false;
	}

	return true;
}

bool TCPSocket::Listen()
{
	// listen()
	int retval = listen(m_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		err_quit("listen()");
		return false;
	}
	return true;
}

void TCPSocket::CreateAcceptSock()	//AcceptSock() -> CreateAcceptSock으로 명칭 변경
{
	//어셉트 소켓 생성. m_sock를 accept소켓으로 초기화.

	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock == INVALID_SOCKET) err_quit("socket()");
}


TCPSocket* TCPSocket::Accept()	//아래 Connect함수의 매개변수 _serverAddr와 다를까?
{
	int addrlen;
	SOCKET _new_sock;
	SOCKADDR_IN clientaddr;
	// accept()
	addrlen = sizeof(clientaddr);
	//서버와 연결된 새로운 소켓을 반환.
	//1 서버의 소켓, 2 클라 주소를 반환하는 출력용 파라미터, 3 두번째 파라미터의 길이
	_new_sock = accept(m_sock, (SOCKADDR*)&clientaddr, &addrlen);	// accept는 소켓을 만드는 것
	if (_new_sock == INVALID_SOCKET) {
		err_display("accept()");
		//예외처리
	}

	printf("[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return new TCPSocket(_new_sock);	// 서버 소켓에 연결을 수락받은(accept된) 클라 소켓의 핸들(_new_sock)으로
										// 내가 새롭게 정의한 TCPSocket클래스 형식의 클라 소켓을 새롭게 생성
}

bool TCPSocket::Recv()
{
	int		retval;
	DWORD	recvbytes;
	DWORD	flags = 0;
	WSABUF	wsabuf;

	ZeroMemory(&m_recv_overlapped.overlapped, sizeof(m_recv_overlapped.overlapped));

	wsabuf.buf = m_Recv_buffer.buf + m_Recv_buffer.comp_bytes;

	if (r_sizeflag)
	{
		wsabuf.len = m_Recv_buffer.target_bytes - m_Recv_buffer.comp_bytes;
	}
	else
	{
		wsabuf.len = sizeof(int) - m_Recv_buffer.comp_bytes;
	}

	retval = WSARecv(m_sock, &wsabuf, 1, &recvbytes,
		&flags, &m_recv_overlapped.overlapped, NULL);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			err_display("WSARecv()");
			return false;
		}
	}

	return true;
}

E_RESULT TCPSocket::CompleteRecv(int _completebyte)
{
	
	if (!r_sizeflag)
	{
		m_Recv_buffer.comp_bytes += _completebyte;

		if (m_Recv_buffer.comp_bytes == sizeof(int))
		{
			memcpy(&m_Recv_buffer.target_bytes, m_Recv_buffer.buf, sizeof(int));
			m_Recv_buffer.comp_bytes = 0;
			r_sizeflag = true;
		}

		if (!Recv())
		{
			return E_RESULT::SOC_ERROR;
		}
				
		return E_RESULT::SOC_FALSE;
	}

	m_Recv_buffer.comp_bytes += _completebyte;

	if (m_Recv_buffer.comp_bytes == m_Recv_buffer.target_bytes)
	{
		m_Recv_buffer.comp_bytes = 0;
		m_Recv_buffer.target_bytes = 0;
		r_sizeflag = false;

		return E_RESULT::SOC_TRUE;
	}
	else
	{
		if (!Recv())
		{
			return E_RESULT::SOC_ERROR;
		}

		return E_RESULT::SOC_FALSE;
	}
}

bool TCPSocket::Send(char* _sendData, int _sendLen)		//send,recv 패킹 클래스로 옮기는 것 고려해볼 것, 패킹 클래스 만들것
{
	int retval;
	DWORD sendbytes;
	DWORD flags;
	WSABUF wsabuf;

	Send_Buffer* tmp_send_buffer;

	//재전송시 아래 동적할당 하는 부분 스킵
	//tmp_send_buffer이 비어있다면. 아직 동적할당을 받지 않았고 처음 전송을 시작한다는 뜻
	if (_sendLen != 0)
	{
		if (tmp_send_buffer == nullptr)
		{
			//큐에 넣을 구조체 동적 할당
			tmp_send_buffer = new Send_Buffer;

			//구조체의 포인터 버퍼에 주소를 넘겨줄 임시 메모리를 동적 할당
			char* tmpbuf = new char[_sendLen];

			//임시 메모리에 원본 데이터 복사.				//버퍼를 만들어서 복사하지 말고 바로 매개변수의 _sendData를 바로 tmp_send_buffer->buf에 넘기면 안될까? 1
			memcpy(tmpbuf, _sendData, _sendLen);

			//임시 메모리의 시작 주소를 구조체의 포인터 버퍼에 삽입
			tmp_send_buffer->buf = tmpbuf;

			//구조체에 목표 용량 삽입
			tmp_send_buffer->target_bytes = _sendLen;

			//구조체에 완료 용량 삽입
			tmp_send_buffer->comp_bytes = *(_sendData + sizeof(char*) + sizeof(int));
		}

	}


	if (m_Send_buffer.size() == 0)
	{
		m_Send_buffer.push(tmp_send_buffer);
		//그냥 바로 보내면 아래 컴플리트 샌드에서 큐를 확인하는 과정에서 오류가 날 수 있음
	}
	else
	{
		m_Send_buffer.push(tmp_send_buffer);
		//큐에 내용 저장
		return true;
	}

	//구조체 동적 할당 받기
	//sendData를 담을 임시메모리 sendLen만큼 동적할당
	//동적할당 받은 임시메모리의 시작 주소 구조체에 담기
	//큐 확인 후 이미 담겨져 있는 내용이 없으면 바로 샌드
	//이미 담겨져 있는 내용 있으면 대기


	ZeroMemory(&m_send_overlapped.overlapped, sizeof(m_send_overlapped.overlapped));
	/*if (tmp_send_buffer->target_bytes == 0)
	{
		tmp_send_buffer->target_bytes = _sendLen;
	}*/

	wsabuf.buf = tmp_send_buffer->buf + tmp_send_buffer->comp_bytes;
	wsabuf.len = tmp_send_buffer->target_bytes - tmp_send_buffer->comp_bytes;

	retval = WSASend(m_sock, &wsabuf, 1, &sendbytes,
		0, &m_send_overlapped.overlapped, NULL);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			err_display("WSASend()");
			//RemoveClientInfo(_ptr);
			return false;
		}
	}

	//CompleteSend(retval);

	return true;
}

E_RESULT TCPSocket::CompleteSend(int _completebyte)
{
	//큐에서 프론트로 하나 복사하여 가져오기
	Send_Buffer* tmp_send_buffer = m_Send_buffer.front();

	tmp_send_buffer->comp_bytes += _completebyte;
	if (tmp_send_buffer->comp_bytes == tmp_send_buffer->target_bytes)
	{
		//딜리트 2번, 팝, 뒤에 큐 확인 후 있으면 샌드
		delete[] tmp_send_buffer->buf;
		delete tmp_send_buffer;
		m_Send_buffer.pop();
		if (m_Send_buffer.size() != 0)
		{
			//send의 매개변수를 주기 위해 방금 딜리트 했던 tmp_send_buffer를 다시 만들어서 사용함. 뭔가 낭비되는 것 같아서 아쉽다. 2
			Send_Buffer* tmp_send_buffer = m_Send_buffer.front();
			Send(tmp_send_buffer->buf, tmp_send_buffer->target_bytes - tmp_send_buffer->comp_bytes);	// 수정 완료
		}

		return E_RESULT::SOC_TRUE;	//큐에 여러 내용이 있다면. 일부는 성공하고 일부는 실패 할 수 있는데. 이런 경우 return값을 어떻게 반환해줄까. 3
	}

	if (!Send(tmp_send_buffer->buf, 0))
	{
		return E_RESULT::SOC_ERROR;
	}

	return E_RESULT::SOC_FALSE;
}