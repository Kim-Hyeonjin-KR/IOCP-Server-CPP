#include "LoginManager.h"
#include "LoginSession.h"

#define ID_ERROR_MSG(X) "#X 없는아이디\n"	//실제로 이렇게 안쓴다고 함. 일단 보류.
#define ID_ERROR_MSG "없는아이디\n"
#define PW_ERROR_MSG "잘못된패스워드.\n"
#define LOGIN_SUCCESS_MSG "로그인.\n"
#define ID_EXIST_MSG "아이디중복.\n"
#define JOIN_SUCCESS_MSG "가입완료.\n"
#define LOGOUT_MSG "로그아웃.\n"


void LoginManager::Create()
{
	//InitSystem 싱글톤 객체 생성
	//InitSystem test_InitSystem;

	InitSystem::GetInstance()->Create();
	IOCP::Create_IOCPPort();
}

void LoginManager::Init(int p_val)
{
	InitSystem::GetInstance()->Init();
}

void LoginManager::Run()
{

	while (1)
	{
		printf("반복중");

		if (!InitSystem::GetInstance()->Run())
		{
			break;
		}


		//InitSystem::Run이 성공적으로 끝나면 (== IOCP와 Accept소켓이 연결됐다면) 아래 반복문 실행

		//SOCKET sock=accept();

		// InitSystem::Run()으로 이동
		//WSAOVERLAPPED_EX* ptr = new WSAOVERLAPPED_EX;
		//ZeroMemory(ptr, sizeof(WSAOVERLAPPED_EX));
		//ptr->type = E_IO_TYPE::IO_Accept;
		//ptr->_ptr = sock;
		//PostQueuedCompletionStatus();
	}
}




void LoginManager::accepted(void* _ptr)
{
	//_ptr의 소켓 핸들을 사용하여 섹션 객체 생성. 핸들을 넘기는데 _ptr보다 더 좋은 변수명은 없을까? 어찌보면 객체를 가르킨다는 의미는 같다만..
	LoginSession* tmp_LoginSession_ptr = new LoginSession(_ptr);
	m_LoginSession_List.push_back(tmp_LoginSession_ptr); 
	//ipcp port에 등록
	//최초 리시브 날리기
}

//메세지를 수신 완료한 이후 처리할 내용들
//_object의 내용 : 	_ptr
void LoginManager::recved(void* _ptr, int _bytes)
{
	// 매개변수로 오는 void* _ptr이 soc.h의 TCPSocket 클래스의 주소인줄 알았는데
	// 그게 아니라 LoginSession의 클래스의 주소를 넘겨주고. 상황에 맞는 세션을 그때마다 overlapped_ex구조체에 ptr에 넣어줌으로써
	// 어떤 작업을 해야할지 알아냄과 동시에 부모 클래스인 TCPSocket에서 수신한 데이터에 접근하여 사용한다

	// 라고 생각했는데 정확히는 기존 void* _ptr은 soc.h의 TCPSocket 클래스의 주소가 맞다.
	// accepted에서 TCPSocket 클래스의 주소를 사용하여 세션 객체를 만들고
	// 여기서 void* _ptr에 해당 세션 객체의 주소를 넘겨줘서 사용한다.

	// 라고 생각했는데!!! void* _ptr은 soc.h의 핸들이다. TCPSocket 클래스의 주소. 즉 객체의 주소가 핸들이긴 하지만
	// 그냥 포인터라고 하는거랑 핸들은 의미가 좀 다르니 명확하게 하자.
	// 이렇게 객체를 포인터를 사용해서 관리하지 않고. 단순하게 번호를 붙여서 관리하는 방법이 핸들이다.
	// 속으로 뜻하는 의미는 비슷하지만 다르다.
	// 따라서 더이상 세션 객체의 주소를 넘길 필요가 없다. soc의 헨들 번호를 기준으로 LoginManager에서 관리하는 m_LoginSession_List에 접근하여
	// 원하는 LoginSession객체를 가져올 수 있기 때문이다.

	
	//LoginSession 찾기
	std::list<LoginSession*>::iterator iter;
	LoginSession* tmp_LoginSession_ptr = nullptr;

	for (iter = m_LoginSession_List.begin(); iter != m_LoginSession_List.end(); iter++)
	{
		if ((*iter)->GetHandle() == _ptr)
		{
			tmp_LoginSession_ptr = *iter;
		}
	}

	E_LOGIN_MANAGER_PROTOCOL protocol;

	char* tmp_buffer = tmp_LoginSession_ptr->GetRecvBuf();		// 깊은 복사로 봐꾸려면 뭘 해야할지는 일단 다 기능부터 완성시키고 생각하자..
	tmp_LoginSession_ptr->GetProtocol(tmp_buffer, (E_PROTOCOL&)protocol);
	_User_Info* tmp_User_Info;


	switch (tmp_LoginSession_ptr->GetState())
	{
	case E_STATE::MENU_SELECT_STATE:

		switch (protocol)
		{
		case E_LOGIN_MANAGER_PROTOCOL::JOIN_INFO:
			memset(tmp_User_Info, 0, sizeof(_User_Info));
			tmp_LoginSession_ptr->UnPackPacket(tmp_buffer + sizeof(int) + sizeof(int), tmp_User_Info->id, tmp_User_Info->pw, tmp_User_Info->nickname);
			JoinProcess(tmp_User_Info, tmp_LoginSession_ptr);
			tmp_LoginSession_ptr->SetState(E_STATE::JOIN_RESULT_SEND_STATE);

			break;
		case E_LOGIN_MANAGER_PROTOCOL::LOGIN_INFO:
			memset(tmp_User_Info, 0, sizeof(_User_Info));
			tmp_LoginSession_ptr->UnPackPacket(tmp_buffer + sizeof(int) + sizeof(int), tmp_User_Info->id, tmp_User_Info->pw);
			LoginProcess(tmp_User_Info, tmp_LoginSession_ptr);
			tmp_LoginSession_ptr->SetState(E_STATE::LOGIN_RESULT_SEND_STATE);
			break;
		}
		break;
	case E_STATE::LOGIN_STATE:
		switch (protocol)
		{
		case E_LOGIN_MANAGER_PROTOCOL::LOGOUT:
			//memset(tmp_User_Info, 0, sizeof(_User_Info));
			//tmp_LoginSession_ptr->UnPackPacket(tmp_buffer + sizeof(int) + sizeof(int), tmp_User_Info->id, tmp_User_Info->pw);
			LogoutProcess(tmp_LoginSession_ptr);
			tmp_LoginSession_ptr->SetState(E_STATE::LOGOUT_RESULT_SEND_STATE);
			break;
		}
	}
}

//메세지를 전송 완료한 이후 처리할 내용들
void LoginManager::sent(void* _ptr, int _bytes)
{
	std::list<LoginSession*>::iterator iter;
	LoginSession* tmp_LoginSession_ptr = nullptr;

	for (iter = m_LoginSession_List.begin(); iter != m_LoginSession_List.end(); iter++)
	{
		if ((*iter)->GetHandle() == _ptr)
		{
			tmp_LoginSession_ptr = *iter;
		}
	}

	E_LOGIN_MANAGER_PROTOCOL protocol;

	char* tmp_buffer = tmp_LoginSession_ptr->GetRecvBuf();
	tmp_LoginSession_ptr->GetProtocol(tmp_buffer, (E_PROTOCOL&)protocol);
	_User_Info* tmp_User_Info;


	switch (tmp_LoginSession_ptr->GetState())
	{
	case E_STATE::MENU_SELECT_STATE:

		switch (protocol)
		{
		case E_LOGIN_MANAGER_PROTOCOL::JOIN_INFO:
			memset(tmp_User_Info, 0, sizeof(_User_Info));
			tmp_LoginSession_ptr->UnPackPacket(tmp_buffer + sizeof(int) + sizeof(int), tmp_User_Info->id, tmp_User_Info->pw, tmp_User_Info->nickname);
			JoinProcess(tmp_User_Info, tmp_LoginSession_ptr);
			tmp_LoginSession_ptr->SetState(E_STATE::JOIN_RESULT_SEND_STATE);

			break;
		case E_LOGIN_MANAGER_PROTOCOL::LOGIN_INFO:
			memset(tmp_User_Info, 0, sizeof(_User_Info));
			tmp_LoginSession_ptr->UnPackPacket(tmp_buffer + sizeof(int) + sizeof(int), tmp_User_Info->id, tmp_User_Info->pw);
			LoginProcess(tmp_User_Info, tmp_LoginSession_ptr);
			tmp_LoginSession_ptr->SetState(E_STATE::LOGIN_RESULT_SEND_STATE);
			break;
		}
		break;
	case E_STATE::LOGIN_STATE:
		switch (protocol)
		{
		case E_LOGIN_MANAGER_PROTOCOL::LOGOUT:
			//tmp_LoginSession_ptr->PackPacket(샌드 버퍼, LOGOUT_SUCCESS, LOGOUT_MSG, 사이즈);	//샌드 버퍼를 세션에 임시 버퍼로 하나 만들어서 사용하는건 어떨까?
			tmp_LoginSession_ptr->SetState(E_STATE::LOGOUT_RESULT_SEND_STATE);
			break;
		}
	}
}

void LoginManager::disconnect(void* _ptr)
{
	// m_LoginSession_List에서 _ptr을 사용하여 만든 Session객체를 찾아서 주소를 가지고 있는다.
	// 해당 내용을 리스트에서 제거한다
	// 가지고 있던 Session객체의 주소를 메모리 해제한다.
}



BOOL LoginManager::SearchFile(char* _filename)
{


	return 0;
}

bool LoginManager::FileDataLoad()
{
	return false;
}

bool LoginManager::FileDataAdd(_User_Info* _info)
{
	return false;
}

void LoginManager::JoinProcess(_User_Info* _info, LoginSession* _ptr)
{
	E_LOGIN_MANAGER_RESULT join_result = E_LOGIN_MANAGER_RESULT::NODATA;
	char msg[BUFSIZE];
	int size;
	std::list<_User_Info*>::iterator iter;


	//아이디 중복 검사
	for (iter = m_Join_List.begin(); iter != m_Join_List.end(); iter++)
	{

		if (!strcmp((*iter)->id, _info->id))
		{
			join_result = E_LOGIN_MANAGER_RESULT::ID_EXIST;
			strcpy(msg, ID_EXIST_MSG);
			break;
		}
	}

	//아이디가 중복되지 않음. 새로운 유저 정보 등록
	if (join_result == E_LOGIN_MANAGER_RESULT::NODATA)
	{
		_User_Info* user = new _User_Info;
		memset(user, 0, sizeof(_User_Info));
		strcpy(user->id, _info->id);
		strcpy(user->pw, _info->pw);
		strcpy(user->nickname, _info->nickname);
		FileDataAdd(user);
		m_Join_List.push_back(user);
		join_result = E_LOGIN_MANAGER_RESULT::JOIN_SUCCESS;
		strcpy(msg, JOIN_SUCCESS_MSG);
	}

	memset(_info, 0, sizeof(_User_Info));

	_ptr->SetState(E_STATE::JOIN_RESULT_SEND_STATE);
	_ptr->PackPacket(m_tmpbuf, join_result, msg, size);

	if (!_ptr->SendPack(m_tmpbuf))
	{
		_ptr->SetState(E_STATE::DISCONNECTED_STATE);
		return;
	}

}

void LoginManager::LoginProcess(_User_Info* _info, LoginSession* _ptr)
{
	E_LOGIN_MANAGER_RESULT login_result = E_LOGIN_MANAGER_RESULT::NODATA;
	char msg[BUFSIZE];
	int size;
	std::list<_User_Info*>::iterator iter;


	for (iter = m_Join_List.begin(); iter != m_Join_List.end(); iter++)
	{
		if (!strcmp((*iter)->id, _info->id))
		{
			if (!strcmp((*iter)->pw, _info->pw))
			{
				login_result = E_LOGIN_MANAGER_RESULT::LOGIN_SUCCESS;
				strcpy(msg, LOGIN_SUCCESS_MSG);
				//일단 로그인 리스트에 추가하되, E_STATE::DISCONNECTED_STATE가 되면 리스트에서 제거할 수 있도록 할 것
				m_LoginSession_List.push_back(_ptr);
				break;
			}
			else
			{
				login_result = E_LOGIN_MANAGER_RESULT::PW_ERROR;
				strcpy(msg, PW_ERROR_MSG);
				break;
			}
		}
		else
		{
			login_result = E_LOGIN_MANAGER_RESULT::ID_ERROR;
			strcpy(msg, ID_ERROR_MSG);
			break;
		}
	}

	memset(_info, 0, sizeof(_User_Info));

	_ptr->PackPacket(m_tmpbuf, login_result, msg, size);

	if (!_ptr->SendPack(m_tmpbuf))
	{
		_ptr->SetState(E_STATE::DISCONNECTED_STATE);
		return;
	}
}

void LoginManager::LogoutProcess(LoginSession* _ptr)
{
	E_LOGIN_MANAGER_RESULT logout_result = E_LOGIN_MANAGER_RESULT::NODATA;
	char msg[BUFSIZE];
	int size;
	std::list<LoginSession*>::iterator iter;



	//for (auto item : m_LoginSession_List)
	//{
	//	item->GetAddr();
	//}
	for (iter = m_LoginSession_List.begin(); iter != m_LoginSession_List.end(); iter++)
	{
		//if (_ptr == iter) 포인터는 오퍼레이터 연산자로 비교할 수 없다고 함.

		//if (_ptr->GetAddr() == *iter->GetAddr())		//**iter이 포인터 리스트의 포인터라서 이중 포인터다. 이러면 어떻게 사용하지?
		{
			//if (_ptr->GetHandle() == iter->GetHandle())
			{
				logout_result = E_LOGIN_MANAGER_RESULT::LOGOUT_SUCCESS;
				strcpy(msg, LOGIN_SUCCESS_MSG);
				//먼저 로그아웃 처리 한 이후 처리된 내용을 보냄. 선조치후보고
				m_LoginSession_List.remove(_ptr);
				//break;
			}

			//LoginSession* tempval = *iter;
			//tempval->GetAddr();

			(*iter)->GetAddr();
		}

	}
	//**로그아웃에 실패하는 경우가 있을까?


	_ptr->PackPacket(m_tmpbuf, logout_result, msg, size);

	if (!_ptr->SendPack(m_tmpbuf))
	{
		_ptr->SetState(E_STATE::DISCONNECTED_STATE);
		return;
	}
}




