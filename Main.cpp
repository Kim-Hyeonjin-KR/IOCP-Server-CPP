#include "global.h"
#include "LoginManager.h"	// global.h�� �Ŵ����� �߰��ϸ� LoginSession�� �Ŵ����� ���谡 ���� �����ϴ� �������� ��������.

int main()
{
	//if (!InitSystem::Init(SERVERPORT))
	//{
	//	printf("�ʱ�ȭ ����\n");
	//	return;
	//}

	////�ʱ�ȭ ���� accept�ݺ� ����
	////accept ���� �Ź� Ȥ�� �ѹ� ���� ����
	//while (true)
	//{
	//	//accept

	//	//����
	//}
	
	LoginManager::GetInstance()->Create();
	LoginManager::GetInstance()->Init();
	LoginManager::GetInstance()->Run();
	//LoginManager::GetInstance->End();
	//LoginManager::Destroy();
}