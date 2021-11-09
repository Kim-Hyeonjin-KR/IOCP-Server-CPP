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

//전방선언 예시
//class LoginSession;


class LoginManager : public IOCP, public TemplateSingleton<LoginManager>
{
public:
	void Create();
	void Init(int p_val = 0);	//p_val ??

private:
	//지역변수는 지역변수로 쓰는게 좋음
	//싱글톤으로 변경
	std::list<_User_Info*> m_Join_List;
	std::list<LoginSession*> m_LoginSession_List;
	char m_tmpbuf[BUFSIZE + 1];		// 매니저에서 입력받은 순수한 데이터를 담을 작업대. 로그인시스템에 넘겨준다.

public:
	void Run();//accept() 
	void accepted(void* _object);
	void recved(void* _object, int _bytes);
	void sent(void* _object, int _bytes);
	void disconnect(void* _object);

	//init함수. 노트 참조

	BOOL SearchFile(char* _filename);
	bool FileDataLoad();
	bool FileDataAdd(_User_Info* _info);
	void JoinProcess(_User_Info* _info, LoginSession* _ptr);
	void LoginProcess(_User_Info* _info, LoginSession* _ptr);
	void LogoutProcess(LoginSession* _ptr);
};