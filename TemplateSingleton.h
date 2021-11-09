#pragma once

template<typename T>
class TemplateSingleton
{
private:
	static T* m_pInstance;
protected:
	TemplateSingleton();
	~TemplateSingleton();

public:
	static T* GetInstance();
	static void DestroyInstance();
};


template <typename T>
T* TemplateSingleton<T>::m_pInstance = nullptr;

template<typename T>
TemplateSingleton<T>::TemplateSingleton()
{
}

template<typename T>
TemplateSingleton<T>::~TemplateSingleton()
{
}


template<typename T>
T* TemplateSingleton<T>::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new T;
	}
	return m_pInstance;
}

template<typename T>
void  TemplateSingleton<T>::DestroyInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}