#include "global.h"
#include "LoginManager.h"	// global.h에 매니저를 추가하면 LoginSession과 매니저의 관계가 서로 참조하는 형식으로 꼬여버림.

int main()
{
	//if (!InitSystem::Init(SERVERPORT))
	//{
	//	printf("초기화 실패\n");
	//	return;
	//}

	////초기화 이후 accept반복 수행
	////accept 이후 매번 혹은 한번 세션 실행
	//while (true)
	//{
	//	//accept

	//	//세션
	//}
	
	LoginManager::GetInstance()->Create();
	LoginManager::GetInstance()->Init();
	LoginManager::GetInstance()->Run();
	//LoginManager::GetInstance->End();
	//LoginManager::Destroy();
}