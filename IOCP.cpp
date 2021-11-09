#include "IOCP.h"

// https://sanghun219.tistory.com/104
// IOCP�� �ϳ��� ť �ΰ�? OS�� ����Ǿ� �ְ�. ��Ʈ�� �־ ���� ���Ͽ� ��Ʈ�� Ŭ�� ������ ��Ʈ�� ����
// �Ǿ� �ִ� �ǰ�?

bool IOCP::Create_IOCPPort()
{
	// ����� �Ϸ� ��Ʈ ����
	m_hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_hcp == NULL) return false;

	// CPU ���� Ȯ��
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// (CPU ���� * 2)���� �۾��� ������ ����
	HANDLE hThread;
	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; i++) {
		//CreateThread	���ϰ�	 : Ŀ��() ������Ʈ�� �ڵ�
		//		"		�Ű����� : 1 ���� ���� ����. ����Ʈ ������ ���� NULL
		//						 : 2 ������ ���� �� �䱸�Ǵ� ������ ũ��. 0�̸� ����Ʈ ������ ������ ũ�⸦ �Ҵ����.
		//						 : 3 ������� ���� �Լ��� ������
		//						 : 4 3���� ���� �Լ��� ������ �Ű�����
		//						 : 5 ������ ���� ���� ��� ���� ���� or ��� ���� ����. 0�̸� �ٷ� ���� ����
		//						 : 6 ������ ���� �� �������� ID. �̸� �����ϱ� ���� ������ ���� ������.
		//						 : �ǹ�. 6���� �ǹ̴� return�Ǵ� �ڵ� ���� ���� �ǹ̰� �ٸ���?
		hThread = CreateThread(NULL, 0, WorkerThread, this, 0, NULL);
		if (hThread == NULL) return false;
		//CloseHandle(hThread); // ��� ������ ������ �� �۾� ���ֱ�
		//�ڵ� ���Ϳ� �־��ֱ�
		m_hThread_list.push_back(hThread);
	}
	return true;
}

DWORD __stdcall IOCP::WorkerThread(void* _ptr)
{
	static_cast<IOCP*>(_ptr)->Running();
	return 0;
}


//������, ���� ��ȣ, ��������, ��ŷ�� ������
// 									= ������ : ������, ������ ��������, ���, ���� ������
//������ ��ŷ�Ǿ� �ִµ�. �Ŵ��� ���������� ��� �����Ͽ��� �ϴ°�.
//OverlappedEX�� ���ԵǾ� �ִ� void* ptr�� �����Ͽ� �����ϴ°��� �´°�.
//�´ٸ� �Ʒ� Running()�Լ����� �ش� ���������� ã�Ƽ� �����ϴ°��� �´°�.

void IOCP::Running()
{
	//dword, long*, �������ǵ�ex����� �������ǵ��ĳ����, ���Ǵ�Ʈ
	DWORD data_size;
	unsigned long ptr;
	WSAOVERLAPPED_EX* overlapped_ex;

	while (true)
	{
		// I/O �Ϸ���Ʈ���� ��Ŷ�� �������� �Լ�
												//�Ϸ���Ʈ�� ���
												//����� �Ϸ� ������ ����� ���۵� ������ ũ��
												//���� ���� ���õ� �Ϸ� Ű���� ������
												//overlapped ������
												//�Ϸ� ��Ŷ�� ��ٷ��ִ� �ð�
		bool retval = GetQueuedCompletionStatus(m_hcp, &data_size, &ptr, (LPOVERLAPPED*)&overlapped_ex, INFINITE);


		//�����˻�
		if (retval == false)
		{
			disconnect(overlapped_ex->_ptr);
		}



		//GetQued�� 4��° �Ű��������� IO_TYPE���� �����Ͽ� �˾Ƴ� ������ �Լ� ȣ��.
		switch (overlapped_ex->type)
		{
		case E_IO_TYPE::IO_Recv :
			recved(overlapped_ex->_ptr, (int)data_size);
			break;

		case E_IO_TYPE::IO_Send :
			sent(overlapped_ex->_ptr, (int)data_size);
			break;

		case E_IO_TYPE::IO_Accept :
			accepted(overlapped_ex->_ptr); //loginsseion ��ü  ������ iocp ��Ʈ�� �����ڵ� ���
			break;

		default:
			break;
		}
	}
}


bool	IOCP::RegisterIocp(SOCKET _sock)
{
	HANDLE result_handle;
	// ���� ���� Ŭ�� ������ IOCP����� ��Ʈ�� �������ݴϴ�.
	result_handle = CreateIoCompletionPort((HANDLE)_sock, m_hcp, NULL, 0);	//CompletionKey ��� overlapped_ex����ü�� ����ϱ� ������ NULL���� �Ű������� ���ϴ�.
													//**InitSystem���� ȣ���ϱ� ���� �޼��带 static���� ������� ������. m_hcp���� static�̿��� �Ѵ�.
	if (result_handle == NULL)
		return false;


	return true;
}

bool	IOCP::PostQueuedNewAccpetSock(WSAOVERLAPPED_EX* _ptr)
{
	return PostQueuedCompletionStatus(m_hcp, 0, NULL, (LPOVERLAPPED)_ptr);
}

HANDLE IOCP::GetHandle()
{

	return m_hcp;
}

