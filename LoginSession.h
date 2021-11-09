#pragma once

#include "global.h"
#include "Packet.h"

//enum STATE
//{
//	INIT_STATE = -1,
//	MENU_SELECT_STATE = 1,	//enum���� E�ٿ��ֵ��� ����
//	JOIN_RESULT_SEND_STATE,
//	LOGIN_RESULT_SEND_STATE,
//	LOGIN_STATE,
//	LOGOUT_RESULT_SEND_STATE,
//	DISCONNECTED_STATE
//};



class LoginSession : Packet
{
private:
	//state�� ���⼭ ����
	E_STATE		m_state;
	//�ڵ�,ip�� Ŭ�� ������ �� �ֵ��� ��� ���� �߰�
	HANDLE		m_handle;		//���� �ڵ�
	IN_ADDR		m_addr;

public:
	LoginSession();
	LoginSession(void* ptr);
	~LoginSession();

	bool LoginSession::operator==(const LoginSession& other) const
	{
		if (m_handle == other.m_handle)
		{
			//IN_ADDR�� == �����ڰ� ��� �ּ�.
			//if (m_addr == other.m_addr)
			{
				return true;
				//���� �񱳴� ���� ���ص� �ɰŰ��Ƽ� �ּ�.
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

	bool SendPack(const char* _str1);	// ����, ��ŶID, ��������, ������ ������ ��ŷ�մϴ�
	bool RecvPack(char* _recvbuf, char* _str1);

	char* GetRecvBuf();

};