#include "LoginManager.h"
#include "LoginSession.h"

#define ID_ERROR_MSG(X) "#X ���¾��̵�\n"	//������ �̷��� �Ⱦ��ٰ� ��. �ϴ� ����.
#define ID_ERROR_MSG "���¾��̵�\n"
#define PW_ERROR_MSG "�߸����н�����.\n"
#define LOGIN_SUCCESS_MSG "�α���.\n"
#define ID_EXIST_MSG "���̵��ߺ�.\n"
#define JOIN_SUCCESS_MSG "���ԿϷ�.\n"
#define LOGOUT_MSG "�α׾ƿ�.\n"


void LoginManager::Create()
{
	//InitSystem �̱��� ��ü ����
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
		printf("�ݺ���");

		if (!InitSystem::GetInstance()->Run())
		{
			break;
		}


		//InitSystem::Run�� ���������� ������ (== IOCP�� Accept������ ����ƴٸ�) �Ʒ� �ݺ��� ����

		//SOCKET sock=accept();

		// InitSystem::Run()���� �̵�
		//WSAOVERLAPPED_EX* ptr = new WSAOVERLAPPED_EX;
		//ZeroMemory(ptr, sizeof(WSAOVERLAPPED_EX));
		//ptr->type = E_IO_TYPE::IO_Accept;
		//ptr->_ptr = sock;
		//PostQueuedCompletionStatus();
	}
}




void LoginManager::accepted(void* _ptr)
{
	//_ptr�� ���� �ڵ��� ����Ͽ� ���� ��ü ����. �ڵ��� �ѱ�µ� _ptr���� �� ���� �������� ������? ����� ��ü�� ����Ų�ٴ� �ǹ̴� ���ٸ�..
	LoginSession* tmp_LoginSession_ptr = new LoginSession(_ptr);
	m_LoginSession_List.push_back(tmp_LoginSession_ptr); 
	//ipcp port�� ���
	//���� ���ú� ������
}

//�޼����� ���� �Ϸ��� ���� ó���� �����
//_object�� ���� : 	_ptr
void LoginManager::recved(void* _ptr, int _bytes)
{
	// �Ű������� ���� void* _ptr�� soc.h�� TCPSocket Ŭ������ �ּ����� �˾Ҵµ�
	// �װ� �ƴ϶� LoginSession�� Ŭ������ �ּҸ� �Ѱ��ְ�. ��Ȳ�� �´� ������ �׶����� overlapped_ex����ü�� ptr�� �־������ν�
	// � �۾��� �ؾ����� �˾Ƴ��� ���ÿ� �θ� Ŭ������ TCPSocket���� ������ �����Ϳ� �����Ͽ� ����Ѵ�

	// ��� �����ߴµ� ��Ȯ���� ���� void* _ptr�� soc.h�� TCPSocket Ŭ������ �ּҰ� �´�.
	// accepted���� TCPSocket Ŭ������ �ּҸ� ����Ͽ� ���� ��ü�� �����
	// ���⼭ void* _ptr�� �ش� ���� ��ü�� �ּҸ� �Ѱ��༭ ����Ѵ�.

	// ��� �����ߴµ�!!! void* _ptr�� soc.h�� �ڵ��̴�. TCPSocket Ŭ������ �ּ�. �� ��ü�� �ּҰ� �ڵ��̱� ������
	// �׳� �����Ͷ�� �ϴ°Ŷ� �ڵ��� �ǹ̰� �� �ٸ��� ��Ȯ�ϰ� ����.
	// �̷��� ��ü�� �����͸� ����ؼ� �������� �ʰ�. �ܼ��ϰ� ��ȣ�� �ٿ��� �����ϴ� ����� �ڵ��̴�.
	// ������ ���ϴ� �ǹ̴� ��������� �ٸ���.
	// ���� ���̻� ���� ��ü�� �ּҸ� �ѱ� �ʿ䰡 ����. soc�� ��� ��ȣ�� �������� LoginManager���� �����ϴ� m_LoginSession_List�� �����Ͽ�
	// ���ϴ� LoginSession��ü�� ������ �� �ֱ� �����̴�.

	
	//LoginSession ã��
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

	char* tmp_buffer = tmp_LoginSession_ptr->GetRecvBuf();		// ���� ����� ���ٷ��� �� �ؾ������� �ϴ� �� ��ɺ��� �ϼ���Ű�� ��������..
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

//�޼����� ���� �Ϸ��� ���� ó���� �����
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
			//tmp_LoginSession_ptr->PackPacket(���� ����, LOGOUT_SUCCESS, LOGOUT_MSG, ������);	//���� ���۸� ���ǿ� �ӽ� ���۷� �ϳ� ���� ����ϴ°� ���?
			tmp_LoginSession_ptr->SetState(E_STATE::LOGOUT_RESULT_SEND_STATE);
			break;
		}
	}
}

void LoginManager::disconnect(void* _ptr)
{
	// m_LoginSession_List���� _ptr�� ����Ͽ� ���� Session��ü�� ã�Ƽ� �ּҸ� ������ �ִ´�.
	// �ش� ������ ����Ʈ���� �����Ѵ�
	// ������ �ִ� Session��ü�� �ּҸ� �޸� �����Ѵ�.
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


	//���̵� �ߺ� �˻�
	for (iter = m_Join_List.begin(); iter != m_Join_List.end(); iter++)
	{

		if (!strcmp((*iter)->id, _info->id))
		{
			join_result = E_LOGIN_MANAGER_RESULT::ID_EXIST;
			strcpy(msg, ID_EXIST_MSG);
			break;
		}
	}

	//���̵� �ߺ����� ����. ���ο� ���� ���� ���
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
				//�ϴ� �α��� ����Ʈ�� �߰��ϵ�, E_STATE::DISCONNECTED_STATE�� �Ǹ� ����Ʈ���� ������ �� �ֵ��� �� ��
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
		//if (_ptr == iter) �����ʹ� ���۷����� �����ڷ� ���� �� ���ٰ� ��.

		//if (_ptr->GetAddr() == *iter->GetAddr())		//**iter�� ������ ����Ʈ�� �����Ͷ� ���� �����ʹ�. �̷��� ��� �������?
		{
			//if (_ptr->GetHandle() == iter->GetHandle())
			{
				logout_result = E_LOGIN_MANAGER_RESULT::LOGOUT_SUCCESS;
				strcpy(msg, LOGIN_SUCCESS_MSG);
				//���� �α׾ƿ� ó�� �� ���� ó���� ������ ����. ����ġ�ĺ���
				m_LoginSession_List.remove(_ptr);
				//break;
			}

			//LoginSession* tempval = *iter;
			//tempval->GetAddr();

			(*iter)->GetAddr();
		}

	}
	//**�α׾ƿ��� �����ϴ� ��찡 ������?


	_ptr->PackPacket(m_tmpbuf, logout_result, msg, size);

	if (!_ptr->SendPack(m_tmpbuf))
	{
		_ptr->SetState(E_STATE::DISCONNECTED_STATE);
		return;
	}
}




