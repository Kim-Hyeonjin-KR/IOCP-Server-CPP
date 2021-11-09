#pragma once

#include "global.h"
#include "IOCP.h"
#include "LoginSession.h"
#include "InitSystem.h"

#define ID "abcd"
#define PW 1234

#define IDSIZE 255
#define PWSIZE 255
#define NICKNAMESIZE 255

struct _User_Info
{
	char id[IDSIZE];
	char pw[PWSIZE];
	char nickname[NICKNAMESIZE];
};

//���漱�� ����
//class LoginSession;


class LoginManager : public IOCP, public TemplateSingleton<LoginManager>
{
public:
	void Create();
	void Init(int p_val = 0);	//p_val ??

private:
	//���������� ���������� ���°� ����
	//�̱������� ����
	std::list<_User_Info*> m_Join_List;
	std::list<LoginSession*> m_LoginSession_List;
	char m_tmpbuf[BUFSIZE + 1];		// �Ŵ������� �Է¹��� ������ �����͸� ���� �۾���. �α��νý��ۿ� �Ѱ��ش�.

public:
	void Run();//accept() 
	void accepted(void* _object);
	void recved(void* _object, int _bytes);
	void sent(void* _object, int _bytes);
	void disconnect(void* _object);

	//init�Լ�. ��Ʈ ����

	BOOL SearchFile(char* _filename);
	bool FileDataLoad();
	bool FileDataAdd(_User_Info* _info);
	void JoinProcess(_User_Info* _info, LoginSession* _ptr);
	void LoginProcess(_User_Info* _info, LoginSession* _ptr);
	void LogoutProcess(LoginSession* _ptr);
};