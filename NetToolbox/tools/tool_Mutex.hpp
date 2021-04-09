#ifndef __TOOL_MUTEX_HPP__
#define __TOOL_MUTEX_HPP__

#include <Windows.h>
#include <tchar.h>



class tool_Mutex {
public:
	// 互斥量构造
	tool_Mutex (LPCTSTR lpName) {
		m_hMutex = ::CreateMutex (nullptr, FALSE, lpName);
	}

	// 互斥量析构
	~tool_Mutex () {
		if (m_hMutex) {
			if (m_locked)
				unlock ();
			::CloseHandle (m_hMutex);
			m_hMutex = NULL;
		}
	}

	// 加锁
	void lock () {
		if (m_hMutex)
			::WaitForSingleObject (m_hMutex, INFINITE);
		m_locked = true;
	}

	// 尝试加锁
	bool try_lock () {
		if (!m_hMutex)
			return false;
		if (WAIT_TIMEOUT == ::WaitForSingleObject (m_hMutex, 0))
			return false;
		m_locked = true;
		return true;
	}

	// 解锁
	void unlock () {
		if (m_hMutex)
			::ReleaseMutex (m_hMutex);
		m_locked = false;
	}

private:
	HANDLE m_hMutex = NULL;
	bool m_locked = false;
};

#endif //__TOOL_MUTEX_HPP__
