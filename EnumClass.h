#pragma once

// ���� �ʿ��� ������ �ٽ� ������!


enum class E_RESULT
{
	SOC_ERROR = -1,
	SOC_TRUE,
	SOC_FALSE
};

enum class E_IO_TYPE
{
	IO_Recv = 1,
	IO_Send = 2,
	IO_Accept = 3
};


// ����� ���� ��� ��������
enum class E_PROTOCOL
{
	INTRO = 1,
	MAIN,
	PLAY
};

enum class E_LOGIN_MANAGER_RESULT
{
	NODATA = -1,
	ID_EXIST = 1,
	ID_ERROR,
	PW_ERROR,
	JOIN_SUCCESS,
	LOGIN_SUCCESS,
	LOGOUT_SUCCESS
};

enum class E_LOGIN_MANAGER_PROTOCOL
{
	JOIN_INFO,
	LOGIN_INFO,
	JOIN_RESULT,
	LOGIN_RESULT,
	LOGOUT,
	LOGOUT_RESULT
};

enum class E_STATE
{
	INIT_STATE = -1,
	MENU_SELECT_STATE = 1,	//enum���� E�ٿ��ֵ��� ����
	JOIN_RESULT_SEND_STATE,
	LOGIN_RESULT_SEND_STATE, // 123
	LOGIN_STATE,
	LOGOUT_RESULT_SEND_STATE,
	DISCONNECTED_STATE
};