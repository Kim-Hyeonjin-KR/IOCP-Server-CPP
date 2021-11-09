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
	//char* _ptr = _recvbuf + sizeof(int) + sizeof(int);		// ���ú���� ���� + ���ú���� ���� ������(4) + �ø��� �ѹ� ������(4) = �������� �޸� �����ּ�
	char* _ptr = _recvbuf + sizeof(int) + sizeof(int) + sizeof(int);		// ���ú���� ���� + ���ú���� ���� ������(4) + �ø��� �ѹ� ������(4) + ������ ������(4) = �������� �޸� �����ּ�

	memcpy(&_protocol, _ptr, sizeof(E_PROTOCOL));
}


int Packet::PackPacket(char* _sendbuf, const char* _data1)	//_str1 ���� ��ü�� ������ �Ŵ������� ������ ����� ��ŷ�ؼ� ������ ����
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

	// ��Ŷ ID �߰�
	memcpy(ptr, &_packid, sizeof(_packid));
	ptr = ptr + sizeof(_packid);
	size = size + sizeof(_packid);

	// �������� �߰� �κ� �����߽��ϴ�. Size, SN, Data(size, protocol, data1 ... ) ������ ��ŷ�Ͽ� ���������� �ϳ��� ����մϴ�.
	//// �������� �߰�
	//memcpy(ptr, &_protocol, sizeof(_protocol));
	//ptr = ptr + sizeof(_protocol);
	//size = size + sizeof(_protocol);

	// ���� ���� �߰�
	memcpy(ptr, &datasize, sizeof(datasize));
	ptr = ptr + sizeof(datasize);
	size = size + sizeof(datasize);

	// ���� �߰�
	memcpy(ptr, _data1, datasize);
	ptr = ptr + datasize;
	size = size + datasize;


	// ���� �տ� �� �뷮 �߰�
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



//Recv�� ���� ȣ���ϴ°�

char* Packet::GetRecvBuf()
{
	return TCPSocket::m_Recv_buffer.buf;
}

