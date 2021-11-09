#pragma once




// ��� �ּ� �޾Ƽ� �����ϴ°� ����.
// �׷��� �ǹ� 4~5���� �� ���� �ּ� �� �Ⱦ���. �ּ��� �ڵ��� �ؼ��� ������ �� �ִ�.
// �߸��Ȱ� ã�� ����� �� �� �ִ�.

// Ŭ������ ���� �ٽ����� ��� ����� �־�鼭 UML�� ������.
// ���� �� �Լ��� ���� ���� Ŭ������ �������. ���� ������ 1~2�� ������ �߷�����.
// ��ü���� ����1(Ŭ���� ���̾�׷�), ��ü���� �÷ο���Ʈ1 �׷��� 2�� ������ �ȴ�.

// ����Ʈ ������ ���.
// �̱��� ������ ��� ���� �� ������ ���� ��Ȯ�� �ľ��ض�.
// Init�̶� �α��θŴ��� �ϼ���ų��.
// EnumŬ���� ���빰 �����Ұ�.



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
	}		// ����ƽ���� instance�� �����. �� instance�� ���������ΰ�? �ƴϸ� ��������, ���������� ������ �ƴ� 
																		// static�̱� ������ ������ ������ ����ִ� �ϳ��� ������ �����ͷ� ����ؾ��ϴ°�?
																		
																		// �׷��� template���� �ڷ����� �޾Ƽ� ���� instance�� GetInstance��� public �Լ��� ����Ͽ� �����ͼ� ����.
																		// �׷��� �̷��� LoginManager::GetInstance().Init(); ó�� ���°ǵ�. LoginManager::GetInstance()������ �׷��� �ĵ�.
																		// Init()�� ��� Ƣ��°���?
																		
																		// Init()�� ���� ����ű���.
																		// LoginManagerŬ������ GetInstance(�̱����� �θ�� ��ӹ޾Ƽ�. �θ��� public�Լ��� ��� ����)
																		// GetInstance�� ��ȯ�ϴ� ���� LoginManager �� ��ü�̱� ������ �翬�� LoginManager�� init()�� ��� ����. ��,
																		// �ᱹ �׳� LoginManager::init()�� ���µ� �̰� �̱������� ������ �߰��� GetInstance�� �ִ°��̴�.

																		// �̰� ��� ���� ������ ���� �����϶�� �ϼ̴µ�.. �̱����� ����ϴ°� �ƴ϶� �ٸ� ������� ���� �϶�°ɱ�?
																		// �ܼ��ϰ� �����ϸ� �׳� �̱����� ������� ������ �Ŵ����� �׳� �� ������ �ھƹ����� �ɰŰ�����..

public:
	virtual void Init(); // �������̽�

};

