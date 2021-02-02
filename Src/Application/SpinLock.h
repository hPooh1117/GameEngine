#pragma once
#include <atomic>

class SpinLock
{
private:
	std::atomic_flag lck = ATOMIC_FLAG_INIT;

public:
	void Lock()
	{
		while (!TryLock()) {}
	}

	bool TryLock()
	{
		return !lck.test_and_set(std::memory_order_acquire);
	}

	void Unlock()
	{
		lck.clear(std::memory_order_release);
	}
};