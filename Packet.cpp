#include "Packet.h"


int Packet::GetID(char* _recvbuf)
{
	char* _ptr = _recvbuf;
	char* tmpID;

	memcpy(tmpID, _ptr, sizeof(int));

	return atoi(tmpID);
}


void Packet::GetProtocol(char* _recvbuf, E_PROTOCOL& _protocol)
{
	//char* _ptr = _recvbuf + sizeof(int) + sizeof(int);		// 리시브버퍼 시작 + 리시브버퍼 현재 사이즈(4) + 시리얼 넘버 사이즈(4) = 프로토콜 메모리 시작주소
	char* _ptr = _recvbuf + sizeof(int) + sizeof(int) + sizeof(int);		// 리시브버퍼 시작 + 리시브버퍼 현재 사이즈(4) + 시리얼 넘버 사이즈(4) + 데이터 사이즈(4) = 프로토콜 메모리 시작주소

	memcpy(&_protocol, _ptr, sizeof(E_PROTOCOL));
}


int Packet::PackPacket(char* _sendbuf, const char* _data1)	//_str1 내용 자체에 각각의 매니저들이 데이터 사이즈를 패킹해서 보내준 상태
{
	char* ptr = _sendbuf;
	int datasize = strlen(_data1);
	int size = 0;
	int _packid = Send_PacketID++;

	if (_packid == FAILED)
	{
		return FAILED;
	}


	ptr = ptr + sizeof(size);

	// 패킷 ID 추가
	memcpy(ptr, &_packid, sizeof(_packid));
	ptr = ptr + sizeof(_packid);
	size = size + sizeof(_packid);

	// 프로토콜 추가 부분 제거했습니다. Size, SN, Data(size, protocol, data1 ... ) 순으로 패킹하여 프로토콜은 하나만 사용합니다.
	//// 프로토콜 추가
	//memcpy(ptr, &_protocol, sizeof(_protocol));
	//ptr = ptr + sizeof(_protocol);
	//size = size + sizeof(_protocol);

	// 내용 길이 추가
	memcpy(ptr, &datasize, sizeof(datasize));
	ptr = ptr + sizeof(datasize);
	size = size + sizeof(datasize);

	// 내용 추가
	memcpy(ptr, _data1, datasize);
	ptr = ptr + datasize;
	size = size + datasize;


	// 제일 앞에 총 용량 추가
	ptr = _sendbuf;
	memcpy(ptr, &size, sizeof(size));

	size = size + sizeof(size);

	return size;
}

int Packet::UnPackPacket(char* _recvbuf, char* _data1)
{
	int data_size;

	char* ptr = _recvbuf + sizeof(int) + sizeof(E_PROTOCOL);

	memcpy(&data_size, ptr, sizeof(data_size));
	ptr = ptr + sizeof(data_size);

	memcpy(_data1, ptr, data_size);
	ptr = ptr + data_size;

	return data_size;
}

int Packet::Send(char* _sendData, int _inLen)
{
	return TCPSocket::Send(_sendData, _inLen);
}



//Recv는 누가 호출하는가

char* Packet::GetRecvBuf()
{
	return TCPSocket::m_Recv_buffer.buf;
}

