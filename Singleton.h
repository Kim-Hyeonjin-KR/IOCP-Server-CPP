#pragma once




// 길게 주석 달아서 공부하는건 좋다.
// 그런데 실무 4~5년차 쯤 가면 주석 잘 안쓴다. 주석이 코드의 해석을 방해할 수 있다.
// 잘못된걸 찾기 어려워 질 수 있다.

// 클래스별 정말 핵심적인 기능 몇가지만 넣어가면서 UML을 만들어라.
// 내가 이 함수를 쓰기 위해 클래스를 만들었다. 싶을 정도의 1~2개 정도만 추려내라.
// 전체적인 구조1(클래스 다이어그램), 전체적인 플로우차트1 그렇게 2장 정도면 된다.

// 스마트 포인터 써라.
// 싱글턴 여러번 상속 받을 때 문제점 뭔지 정확히 파악해라.
// Init이랑 로그인매니저 완성시킬것.
// Enum클래스 내용물 구분할것.



template<typename T>
class Singleton
{ 
protected:
	static T* m_instance;
	static bool m_ISDestroy;

protected: 
	Singleton() {} 


public: 
	Singleton(const Singleton& c) 
	{
		m_instance = nullptr; 
		m_ISDestroy = false;
	}

	void operator=(const Singleton& c) = delete; 

	static void SAFEDestroy()
	{
		if (m_instance)
		{
			delete m_instance;
			m_instance = nullptr;
		}
		m_ISDestroy = true;
	}

	static T* GetInstance() 
	{  
		if (m_instance == nullptr)
		{
			m_instance = new T();
			m_instance->Init();
		}
		return m_instance;
	}		// 스태틱으로 instance를 만든다. 이 instance는 지역변수인가? 아니면 지역변수, 전역변수의 개념이 아닌 
																		// static이기 때문에 데이터 영역에 들어있는 하나의 별도의 데이터로 취급해야하는가?
																		
																		// 그렇게 template으로 자료형을 받아서 만들어낸 instance를 GetInstance라는 public 함수로 사용하여 가져와서 쓴다.
																		// 그래서 이렇게 LoginManager::GetInstance().Init(); 처럼 쓰는건데. LoginManager::GetInstance()까지는 그렇다 쳐도.
																		// Init()은 어디서 튀어나온거지?
																		
																		// Init()은 내가 만든거구나.
																		// LoginManager클래스의 GetInstance(싱글턴을 부모로 상속받아서. 부모의 public함수를 사용 가능)
																		// GetInstance로 반환하는 값이 LoginManager 그 자체이기 때문에 당연히 LoginManager의 init()을 사용 가능. 즉,
																		// 결국 그냥 LoginManager::init()을 쓰는데 이걸 싱글턴으로 쓰려고 중간에 GetInstance가 있는것이다.

																		// 이걸 상속 문제 때문에 직접 구현하라고 하셨는데.. 싱글턴을 상속하는게 아니라 다른 방식으로 구현 하라는걸까?
																		// 단순하게 생각하면 그냥 싱글턴의 내용들을 각각의 매니저에 그냥 다 가져다 박아버리면 될거같은데..

public:
	virtual void Init(); // 인터페이스

};

