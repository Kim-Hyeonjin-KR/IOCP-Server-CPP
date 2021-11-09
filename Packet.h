#pragma once
#include "global.h"
#include "SOC.h"

class Packet : public TCPSocket
{
private:
	int Send_PacketID;	//1씩 증가
	int Recv_PacketID;	

	//setID삭제

protected:
	int GetID(char* _recvbuf);
	void GetProtocol(char* _ptr, E_PROTOCOL& _protocol);
	int PackPacket(char* _sendbuf, const char* _str1);	// 소켓, 패킷ID, 데이터 순으로 패킹합니다 (프로토콜 제거됨)
	int UnPackPacket(char* _recvbuf, char* _str1);
	int Send(char* _sendData, int _inLen);

	char* Packet::GetRecvBuf();

};

// 여러 인자들을 공통적으로 패킹하기
// 바이트(길이) 내용(바이트)
// 
