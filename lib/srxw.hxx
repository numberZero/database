#pragma once
#include <cstdint>
#include <mutex>
#include <condition_variable>

class SharedReadExclusiveWriteLock
{
	std::mutex mtx;
	std::unique_lock<std::mutex> lock{mtx, std::defer_lock};
	std::condition_variable read_cv;
	std::condition_variable write_cv;
	std::int_fast32_t read_wait = 0;
	std::int_fast32_t read_count = 0;
	std::int_fast32_t write_wait = 0;

public:
	void read_lock();
	void read_unlock();
	void write_lock();
	void write_unlock();
};

typedef SharedReadExclusiveWriteLock SRXWLock;

class SRXW_ReadLockGuard
{
	SRXWLock &lock;
public:
	SRXW_ReadLockGuard(SRXWLock &_lock);
	~SRXW_ReadLockGuard();
};

class SRXW_WriteLockGuard
{
	SRXWLock &lock;
public:
	SRXW_WriteLockGuard(SRXWLock &_lock);
	~SRXW_WriteLockGuard();
};
