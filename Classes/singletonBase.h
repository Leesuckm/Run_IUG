#pragma once

#include <mutex>

#ifndef NULL
#define NULL 0
#endif

template <class T> class CSingletonBase
{
protected:
	CSingletonBase();
	~CSingletonBase();

public:
	static T* getInstance()
	{
		if (!_instance)
		{
			std::lock_guard<std::mutex> lock(_mutex);

			if (!_instance)
				_instance = new T;
		}

		return  _instance;
	}

	static void destroyInstance()
	{
		if (!_instance)
		{
			delete _instance;
			_instance = nullptr;
		}
	}

private:
	static T *			_instance;
	static std::mutex	_mutex;
};

template <class T> T* CSingletonBase<T>::_instance = nullptr;
template <class T> std::mutex CSingletonBase<T>::_mutex;
template <class T> CSingletonBase<T>::CSingletonBase()  {}
template <class T> CSingletonBase<T>::~CSingletonBase() {}
