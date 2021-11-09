#pragma once
#include "global.h"
#include "SOC.h"

class Packet : public TCPSocket
{
private:
	int Send_PacketID;	//1�� ����
	int Recv_PacketID;	

	//setID����

protected:
	int GetID(char* _recvbuf);
	void GetProtocol(char* _ptr, E_PROTOCOL& _protocol);
	int PackPacket(char* _sendbuf, const char* _str1);	// ����, ��ŶID, ������ ������ ��ŷ�մϴ� (�������� ���ŵ�)
	int UnPackPacket(char* _recvbuf, char* _str1);
	int Send(char* _sendData, int _inLen);

	char* Packet::GetRecvBuf();

};

// ���� ���ڵ��� ���������� ��ŷ�ϱ�
// ����Ʈ(����) ����(����Ʈ)
// 
