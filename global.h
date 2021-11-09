#pragma once

#pragma comment(lib, "ws2_32")
#include<winsock2.h>
#include<iostream>

#include<stdlib.h>
#include<memory.h>
#include<list>
#include<queue>

#include"EnumClass.h"
//#include"Singleton.h"
#include"TemplateSingleton.h"

constexpr u_short					SERVERPORT		= 9000;	// constexpr ������ Ÿ�ӿ��� �ʱ�ȭ �Ǿ� ���� ������ ������ ���� ����.
constexpr unsigned __int32			BUFSIZE			= 512;
#define SOCKET_END	0
#define PACKET_NUM	1024	// �ѹ��� ���� �� �� �ִ� ��Ŷ�� �� ����
#define ERROR		-2		// ����� ���� �Լ����� ������ ��� ��ȯ�� ��
#define FAILED		-1		// ����� ���� �Լ����� ������ ��� ��ȯ�� ��

// ����� ���� �Լ� ���� ����
//enum RESULT
//{
//	SOC_ERROR = 1,
//	SOC_TRUE,
//	SOC_FALSE
//};
//
//enum IO_TYPE
//{
//	IO_Recv = 1,
//	IO_Send = 2,
//	IO_Accept
//};
//
//
//// ����� ���� ��� ��������
//enum PROTOCOL
//{
//	INTRO = 1,
//	MAIN,
//	PLAY
//};



// ���� ��� �Լ�
void err_quit(char* msg);
void err_display(char* msg);


