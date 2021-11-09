
#include "LoginSession.h"


LoginSession::LoginSession()
{
}

LoginSession::LoginSession(void* ptr)
{
	//여기서 들어온 soc의 포인터 주소를 어떻게 사용해야 할까
	//요렇게하면 된다! 아하
	m_handle = ptr;
}

LoginSession::~LoginSession()
{
}

E_STATE LoginSession::GetState()
{
	return m_state;
}

void LoginSession::SetState(E_STATE _state)
{
	m_state = _state;
}

HANDLE LoginSession::GetHandle()
{
	return m_handle;
}

IN_ADDR LoginSession::GetAddr()
{
	return m_addr;
}



void LoginSession::PackPacket(char* _buf, E_LOGIN_MANAGER_PROTOCOL _protocol, char* _str1, int& _size)
{
	char* ptr = _buf;
	int strsize1 = strlen(_str1);
	_size = 0;

	ptr = ptr + sizeof(_size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	_size = _size + sizeof(_protocol);

	memcpy(ptr, &strsize1, sizeof(strsize1));
	ptr = ptr + sizeof(strsize1);
	_size = _size + sizeof(strsize1);

	memcpy(ptr, _str1, strsize1);
	ptr = ptr + strsize1;
	_size = _size + strsize1;

	ptr = _buf;
	memcpy(ptr, &_size, sizeof(_size));

	_size = _size + sizeof(_size);
}

void LoginSession::PackPacket(char* _buf, E_LOGIN_MANAGER_RESULT _protocol, char* _str1, int& _size)
{
	char* ptr = _buf;
	int strsize1 = strlen(_str1);
	_size = 0;

	ptr = ptr + sizeof(_size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	_size = _size + sizeof(_protocol);

	memcpy(ptr, &strsize1, sizeof(strsize1));
	ptr = ptr + sizeof(strsize1);
	_size = _size + sizeof(strsize1);

	memcpy(ptr, _str1, strsize1);
	ptr = ptr + strsize1;
	_size = _size + strsize1;

	ptr = _buf;
	memcpy(ptr, &_size, sizeof(_size));

	_size = _size + sizeof(_size);
}

void LoginSession::UnPackPacket(char* _buf, char* _str1, char* _str2, char* _str3)
{
	int str1size, str2size, str3size;

	char* ptr = _buf + sizeof(E_PROTOCOL);

	memcpy(&str1size, ptr, sizeof(str1size));
	ptr = ptr + sizeof(str1size);

	memcpy(_str1, ptr, str1size);
	ptr = ptr + str1size;

	memcpy(&str2size, ptr, sizeof(str2size));
	ptr = ptr + sizeof(str2size);

	memcpy(_str2, ptr, str2size);
	ptr = ptr + str2size;

	memcpy(&str3size, ptr, sizeof(str3size));
	ptr = ptr + sizeof(str3size);

	memcpy(_str3, ptr, str3size);
	ptr = ptr + str3size;
}

void LoginSession::UnPackPacket(char* _buf, char* _str1, char* _str2)
{
	int str1size, str2size;

	char* ptr = _buf + sizeof(E_PROTOCOL);

	memcpy(&str1size, ptr, sizeof(str1size));
	ptr = ptr + sizeof(str1size);

	memcpy(_str1, ptr, str1size);
	ptr = ptr + str1size;

	memcpy(&str2size, ptr, sizeof(str2size));
	ptr = ptr + sizeof(str2size);

	memcpy(_str2, ptr, str2size);
	ptr = ptr + str2size;
}

void LoginSession::GetProtocol(char* _ptr, E_PROTOCOL& _protocol)
{
	Packet::GetProtocol(_ptr, _protocol);
}



bool LoginSession::SendPack(const char* _str1)
{
	int retval;
	char* _sendbuf;	// 보낼 데이터 패킹
	size_t size = strlen(_str1);

	retval = Packet::PackPacket(_sendbuf, _str1);

	if (retval > 0)
	{
		return Packet::Send(_sendbuf, retval);
	}

	return false;
}

bool LoginSession::RecvPack(char* _recvbuf, char* _str1)
{
	int retval;
	size_t size = strlen(_str1);

	retval = Packet::UnPackPacket(_recvbuf, _str1);

	if (retval > 0)
	{
		//return Packet::Recv(_recvbuf, retval);
	}




	return false;
}

char* LoginSession::GetRecvBuf()
{
	return Packet::GetRecvBuf();
}
