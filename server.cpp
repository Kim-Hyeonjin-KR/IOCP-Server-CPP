/*
 * echo_selserv_win.c
 * Written by SW. YOON
 */

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define BUFSIZE 4096
#define IDSIZE 255
#define PWSIZE 255
#define NICKNAMESIZE 255
#define ERROR_DISCONNECTED -2
#define DISCONNECTED -1
#define SOC_TRUE 1
#define SOC_FALSE 0

#define ID_ERROR_MSG "없는 아이디입니다\n"
#define PW_ERROR_MSG "패스워드가 틀렸습니다.\n"
#define LOGIN_SUCCESS_MSG "로그인에 성공했습니다.\n"
#define ID_EXIST_MSG "이미 있는 아이디 입니다.\n"
#define JOIN_SUCCESS_MSG "가입에 성공했습니다.\n"
#define LOGOUT_MSG "로그아웃되었습니다.\n"

enum STATE 
{
	INIT_STATE=-1, 
	MENU_SELECT_STATE=1, 
	JOIN_RESULT_SEND_STATE, 
	LOGIN_RESULT_SEND_STATE,
	LOGIN_STATE,
	LOGOUT_RESULT_SEND_STATE,
	DISCONNECTED_STATE
};

enum RESULT 
{
	NODATA = -1, 
	ID_EXIST = 1, 
	ID_ERROR, 
	PW_ERROR, 
	JOIN_SUCCESS, 
	LOGIN_SUCCESS,
	LOGOUT_SUCCESS
};

enum PROTOCOL 
{ 
	JOIN_INFO, 
	LOGIN_INFO, 
	JOIN_RESULT, 
	LOGIN_RESULT,
	LOGOUT,
	LOGOUT_RESULT
};


struct _User_Info
{
	char id[IDSIZE];
	char pw[PWSIZE];
	char nickname[NICKNAMESIZE];
};

struct _ClientInfo
{
	SOCKET		sock;
	SOCKADDR_IN addr;
	_User_Info  userinfo;
	STATE		state;
	bool		r_sizeflag;
	
	int			recvbytes;
	int			comp_recvbytes;
	int			sendbytes;
	int			comp_sendbytes;

	char		recvbuf[BUFSIZE];
	char		sendbuf[BUFSIZE];

};

_ClientInfo* User_List[100];
int Count = 0;
_User_Info* Join_List[100];
int Join_Count = 0;

FD_SET Rset, Wset;

void err_quit(const char* msg);
void err_display(const char* msg);
int recvn(SOCKET s, char* buf, int len, int flags);
void GetProtocol(const char* _ptr, PROTOCOL& _protocol);
int PackPacket(char* _buf, PROTOCOL _protocol, RESULT _result, const char* _str1);
void UnPackPacket(const char* _buf, char* _str1, char* _str2, char* _str3);
void UnPackPacket(const char* _buf, char* _str1, char* _str2);
BOOL SearchFile(const char* filename);
bool FileDataLoad();
bool FileDataAdd(_User_Info* _info);
int MessageRecv(_ClientInfo* _info);
int MessageSend(_ClientInfo* _info);
int PacketRecv(_ClientInfo* _ptr);
_ClientInfo* AddClient(SOCKET _sock, SOCKADDR_IN _clientaddr);
void RemoveClient(_ClientInfo* _ptr);
void RecvPacketProcess(_ClientInfo* _ptr);
void SendPacketProcess(_ClientInfo* _ptr);
void JoinProcess(_ClientInfo* _ptr);
void LoginProcess(_ClientInfo* _ptr);


int main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET hServSock;
	SOCKADDR_IN servAddr;
	SOCKET hClntSock;
	SOCKADDR_IN clntAddr;
	int retval;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) /* Load Winsock 2.2 DLL */
		err_quit("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		err_quit("socket() error");

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(9000);

	u_long on = 1;
	retval = ioctlsocket(hServSock, FIONBIO, &on);
	if (retval == SOCKET_ERROR) err_display("ioctlsocket()");


	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		err_quit("bind() error");
	if (listen(hServSock, 5) == SOCKET_ERROR)
		err_quit("listen() error");

	if (!FileDataLoad())
	{
		err_quit("file read error!");
	}

	while (1)
	{
		FD_ZERO(&Rset);
		FD_ZERO(&Wset);

		FD_SET(hServSock, &Rset);

		for (int i = 0; i < Count; i++)
		{
			FD_SET(User_List[i]->sock, &Rset);

			switch (User_List[i]->state)
			{
			case JOIN_RESULT_SEND_STATE:
			case LOGIN_RESULT_SEND_STATE:
			case LOGOUT_RESULT_SEND_STATE:
				FD_SET(User_List[i]->sock, &Wset);
				break;
			}			
		}

		if (select(0, &Rset, &Wset, 0, NULL) == SOCKET_ERROR)
		{
			err_quit("select() error");
		}			

		if (FD_ISSET(hServSock, &Rset)) 
		{
			int clntLen = sizeof(clntAddr);
			hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &clntLen);
			_ClientInfo* ptr = AddClient(hClntSock, clntAddr);
			ptr->state = MENU_SELECT_STATE;
			continue;
		}		

		for (int i = 0; i < Count; i++)
		{
			_ClientInfo* ptr = User_List[i];

			if (FD_ISSET(ptr->sock, &Rset))
			{				
				int result = PacketRecv(ptr);

				switch (result)
				{
				case DISCONNECTED:
					ptr->state = DISCONNECTED_STATE;
					break;
				case SOC_FALSE:
					continue;
				case SOC_TRUE:
					break;
				}

				RecvPacketProcess(ptr);
			}					

			if (FD_ISSET(ptr->sock, &Wset))
			{
				int result = MessageSend(ptr);
				switch (result)
				{
				case ERROR_DISCONNECTED:
					err_display("connect end");
				case DISCONNECTED:
					ptr->state = DISCONNECTED_STATE;
					break;
				case SOC_FALSE:
					continue;
				case SOC_TRUE:
					break;
				}

				SendPacketProcess(ptr);				
			}

			if (ptr->state == DISCONNECTED_STATE)
			{
				RemoveClient(ptr);
				i--;
				continue;
			}

		}
	}	

	closesocket(hServSock);
	WSACleanup();
	return 0;
}


void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

void GetProtocol(const char* _ptr, PROTOCOL& _protocol)
{
	memcpy(&_protocol, _ptr, sizeof(PROTOCOL));

}

int PackPacket(char* _buf, PROTOCOL _protocol, RESULT _result, const char* _str1)
{
	char* ptr = _buf;
	int strsize1 = strlen(_str1);
	int size = 0;

	ptr = ptr + sizeof(size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	size = size + sizeof(_protocol);

	memcpy(ptr, &_result, sizeof(_result));
	ptr = ptr + sizeof(_result);
	size = size + sizeof(_result);

	memcpy(ptr, &strsize1, sizeof(strsize1));
	ptr = ptr + sizeof(strsize1);
	size = size + sizeof(strsize1);

	memcpy(ptr, _str1, strsize1);
	ptr = ptr + strsize1;
	size = size + strsize1;

	ptr = _buf;
	memcpy(ptr, &size, sizeof(size));

	size = size + sizeof(size);

	return size;
}

void UnPackPacket(const char* _buf, char* _str1, char* _str2, char* _str3)
{
	int str1size, str2size, str3size;

	const char* ptr = _buf + sizeof(PROTOCOL);

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

void UnPackPacket(const char* _buf, char* _str1, char* _str2)
{
	int str1size, str2size;

	const char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&str1size, ptr, sizeof(str1size));
	ptr = ptr + sizeof(str1size);

	memcpy(_str1, ptr, str1size);
	ptr = ptr + str1size;

	memcpy(&str2size, ptr, sizeof(str2size));
	ptr = ptr + sizeof(str2size);

	memcpy(_str2, ptr, str2size);
	ptr = ptr + str2size;
}

BOOL SearchFile(const char* filename)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile = FindFirstFile(filename, &FindFileData);
	if (hFindFile == INVALID_HANDLE_VALUE)
		return FALSE;
	else {
		FindClose(hFindFile);
		return TRUE;
	}
}

bool FileDataLoad()
{
	if (!SearchFile("UserInfo.info"))
	{
		FILE* fp = fopen("UserInfo.info", "wb");
		fclose(fp);
		return true;
	}

	FILE* fp = fopen("UserInfo.info", "rb");
	if (fp == NULL)
	{
		return false;
	}

	_User_Info info;
	memset(&info, 0, sizeof(_User_Info));

	while (1)
	{
		fread(&info, sizeof(_User_Info), 1, fp);
		if (feof(fp))
		{
			break;
		}
		_User_Info* ptr = new _User_Info;
		memcpy(ptr, &info, sizeof(_User_Info));
		Join_List[Join_Count++] = ptr;
	}

	fclose(fp);
	return true;
}

bool FileDataAdd(_User_Info* _info)
{
	FILE* fp = fopen("UserInfo.info", "ab");
	if (fp == NULL)
	{
		return false;
	}

	int retval = fwrite(_info, 1, sizeof(_User_Info), fp);

	if (retval != sizeof(_User_Info))
	{
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

int MessageRecv(_ClientInfo* _info)
{
	int retval = recv(_info->sock, _info->recvbuf + _info->comp_recvbytes, _info->recvbytes - _info->comp_recvbytes, 0);
	if (retval == SOCKET_ERROR) //강제연결종료요청인 경우
	{
		return ERROR_DISCONNECTED;
	}
	else if (retval == 0)
	{
		return DISCONNECTED;
	}
	else
	{
		_info->comp_recvbytes += retval;
		if (_info->comp_recvbytes == _info->recvbytes)
		{
			_info->comp_recvbytes = 0;
			_info->recvbytes = 0;
			return SOC_TRUE;
		}
		return SOC_FALSE;
	}

}

int MessageSend(_ClientInfo* _info)
{
	int retval = send(_info->sock, _info->sendbuf + _info->comp_sendbytes,
		_info->sendbytes - _info->comp_sendbytes, 0);
	if (retval == SOCKET_ERROR)
	{
		return ERROR_DISCONNECTED;
	}
	else if (retval == 0)
	{
		DISCONNECTED;
	}
	else
	{
		_info->comp_sendbytes = _info->comp_sendbytes + retval;

		if (_info->sendbytes == _info->comp_sendbytes)
		{
			_info->sendbytes = 0;
			_info->comp_sendbytes = 0;

			return SOC_TRUE;
		}
		else
		{
			return SOC_FALSE;
		}
	}
}

int PacketRecv(_ClientInfo* _ptr)
{
	if (!_ptr->r_sizeflag)
	{
		_ptr->recvbytes = sizeof(int);
		int retval = MessageRecv(_ptr);
		switch (retval)
		{
		case SOC_TRUE:
			memcpy(&_ptr->recvbytes, _ptr->recvbuf, sizeof(int));
			_ptr->r_sizeflag = true;
			return SOC_FALSE;
		case SOC_FALSE:
			return SOC_FALSE;
		case ERROR_DISCONNECTED:
			err_display("recv error()");
			return DISCONNECTED;
		case DISCONNECTED:
			return DISCONNECTED;
		}
	}

	int retval = MessageRecv(_ptr);
	switch (retval)
	{
	case SOC_TRUE:
		_ptr->r_sizeflag = false;
		return SOC_TRUE;
	case SOC_FALSE:
		return SOC_FALSE;
	case ERROR_DISCONNECTED:
		err_display("recv error()");
		return DISCONNECTED;
	case DISCONNECTED:
		return DISCONNECTED;
	}
}

_ClientInfo* AddClient(SOCKET _sock, SOCKADDR_IN _clientaddr)
{
	printf("\nClient 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(_clientaddr.sin_addr),
		ntohs(_clientaddr.sin_port));

	//소켓 구조체 배열에 새로운 소켓 정보 구조체 저장
	_ClientInfo* ptr = new _ClientInfo;
	ZeroMemory(ptr, sizeof(_ClientInfo));

	ptr->sock = _sock;
	memcpy(&ptr->addr, &_clientaddr, sizeof(SOCKADDR_IN));
	ptr->state = INIT_STATE;
	ptr->r_sizeflag = false;
	User_List[Count++] = ptr;
	return ptr;
}

void RemoveClient(_ClientInfo* _ptr)
{
	closesocket(_ptr->sock);

	printf("\nClient 종료: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(_ptr->addr.sin_addr), ntohs(_ptr->addr.sin_port));

	for (int i = 0; i < Count; i++)
	{
		if (User_List[i] == _ptr)
		{
			delete User_List[i];

			for (int j = i; j < Count - 1; j++)
			{
				User_List[j] = User_List[j + 1];
			}
			User_List[Count - 1] = nullptr;
			Count--;
			break;
		}
	}

}

void RecvPacketProcess(_ClientInfo* _ptr)
{
	PROTOCOL protocol;

	GetProtocol(_ptr->recvbuf, protocol);

	switch (_ptr->state)
	{
	case MENU_SELECT_STATE:	

		switch (protocol)
		{
		case JOIN_INFO:
			memset(&_ptr->userinfo, 0, sizeof(_User_Info));
			UnPackPacket(_ptr->recvbuf, _ptr->userinfo.id, _ptr->userinfo.pw, _ptr->userinfo.nickname);
			JoinProcess(_ptr);
			_ptr->state = JOIN_RESULT_SEND_STATE;
			break;
		case LOGIN_INFO:
			memset(&_ptr->userinfo, 0, sizeof(_User_Info));
			UnPackPacket(_ptr->recvbuf, _ptr->userinfo.id, _ptr->userinfo.pw);
			LoginProcess(_ptr);
			_ptr->state = LOGIN_RESULT_SEND_STATE;
			break;	
		}
		break;
	case LOGIN_STATE:		
		switch (protocol)
		{		
		case LOGOUT:
			_ptr->sendbytes = PackPacket(_ptr->sendbuf, LOGOUT_RESULT, LOGOUT_SUCCESS, LOGOUT_MSG);
			_ptr->state = LOGOUT_RESULT_SEND_STATE;
			break;

		}

	}

}

void SendPacketProcess(_ClientInfo* _ptr)
{
	PROTOCOL protocol;

	switch (_ptr->state)
	{
	case JOIN_RESULT_SEND_STATE:
		_ptr->state = MENU_SELECT_STATE;
		break;
	case LOGIN_RESULT_SEND_STATE:
		if (strlen(_ptr->userinfo.id) == 0)
		{
			_ptr->state = MENU_SELECT_STATE;
		}
		else
		{
			_ptr->state = LOGIN_STATE;
		}		
		break;
	case LOGOUT_RESULT_SEND_STATE:
		_ptr->state = MENU_SELECT_STATE;
		break;
	}
}

void JoinProcess(_ClientInfo* _ptr)
{
	RESULT join_result = NODATA;

	char msg[BUFSIZE];
	PROTOCOL protocol;

	for (int i = 0; i < Join_Count; i++)
	{
		if (!strcmp(Join_List[i]->id, _ptr->userinfo.id))
		{
			join_result = ID_EXIST;
			strcpy(msg, ID_EXIST_MSG);
			break;
		}
	}

	if (join_result == NODATA)
	{
		_User_Info* user = new _User_Info;
		memset(user, 0, sizeof(_User_Info));
		strcpy(user->id, _ptr->userinfo.id);
		strcpy(user->pw, _ptr->userinfo.pw);
		strcpy(user->nickname, _ptr->userinfo.nickname);

		FileDataAdd(user);

		Join_List[Join_Count++] = user;
		join_result = JOIN_SUCCESS;
		strcpy(msg, JOIN_SUCCESS_MSG);
	}

	protocol = JOIN_RESULT;

	_ptr->sendbytes = PackPacket(_ptr->sendbuf, protocol, join_result, msg);
}

void LoginProcess(_ClientInfo* _ptr)
{
	RESULT login_result = NODATA;

	char msg[BUFSIZE];
	PROTOCOL protocol;

	for (int i = 0; i < Join_Count; i++)
	{
		if (!strcmp(Join_List[i]->id, _ptr->userinfo.id))
		{
			if (!strcmp(Join_List[i]->pw, _ptr->userinfo.pw))
			{
				login_result = LOGIN_SUCCESS;
				strcpy(msg, LOGIN_SUCCESS_MSG);
			}
			else
			{
				login_result = PW_ERROR;
				strcpy(msg, PW_ERROR_MSG);
			}
			break;
		}
	}

	if (login_result == NODATA)
	{
		login_result = ID_ERROR;
		strcpy(msg, ID_ERROR_MSG);
	}

	if (login_result != LOGIN_SUCCESS)
	{
		memset(&(_ptr->userinfo), 0, sizeof(_User_Info));
	}

	protocol = LOGIN_RESULT;
	_ptr->sendbytes = PackPacket(_ptr->sendbuf, protocol, login_result, msg);

}
