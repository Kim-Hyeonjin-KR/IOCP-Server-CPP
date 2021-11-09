#pragma once

#include "global.h"
#include "Packet.h"

//enum STATE
//{
//	INIT_STATE = -1,
//	MENU_SELECT_STATE = 1,	//enum형식 E붙여주도록 변경
//	JOIN_RESULT_SEND_STATE,
//	LOGIN_RESULT_SEND_STATE,
//	LOGIN_STATE,
//	LOGOUT_RESULT_SEND_STATE,
//	DISCONNECTED_STATE
//};



class LoginSession : Packet
{
private:
	//state는 여기서 관리
	E_STATE		m_state;
	//핸들,ip등 클라 구분할 수 있도록 멤버 변수 추가
	HANDLE		m_handle;		//소켓 핸들
	IN_ADDR		m_addr;

public:
	LoginSession();
	LoginSession(void* ptr);
	~LoginSession();

	bool LoginSession::operator==(const LoginSession& other) const
	{
		if (m_handle == other.m_handle)
		{
			//IN_ADDR는 == 연산자가 없어서 주석.
			//if (m_addr == other.m_addr)
			{
				return true;
				//상태 비교는 굳이 안해도 될거같아서 주석.
				//if (m_state == other.m_state)
				//{
				//
				//}
			}
		}

		return false;
	}

	bool LoginSession::operator!=(const LoginSession& other) const
	{
		return !(*this == other);
	}

	E_STATE GetState();
	void SetState(E_STATE _state);
	HANDLE GetHandle();
	IN_ADDR GetAddr();

	void PackPacket(char* _buf, E_LOGIN_MANAGER_PROTOCOL _protocol, char* _str1, int& _size);
	void PackPacket(char* _buf, E_LOGIN_MANAGER_RESULT _protocol, char* _str1, int& _size);
	void UnPackPacket(char* _buf, char* _str1, char* _str2, char* _str3);
	void UnPackPacket(char* _buf, char* _str1, char* _str2);
	void GetProtocol(char* _ptr, E_PROTOCOL& _protocol);

	bool SendPack(const char* _str1);	// 소켓, 패킷ID, 프로토콜, 데이터 순으로 패킹합니다
	bool RecvPack(char* _recvbuf, char* _str1);

	char* GetRecvBuf();

};