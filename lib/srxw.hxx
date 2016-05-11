#pragma once
#include <cstdint>
#include <mutex>
#include <condition_variable>

class SharedReadExclusiveWriteLock
{
	std::mutex mtx;
	std::condition_variable read_cv;
	std::condition_variable write_cv;
	std::int_fast32_t read_wait = 0;
	std::int_fast32_t read_count = 0;
	std::int_fast32_t write_wait = 0;

public:
	typedef SharedReadExclusiveWriteLock Lock;

	class ReadLockGuard
	{
		Lock &srxw;
		std::unique_lock<std::mutex> lock;
	public:
		ReadLockGuard() = delete;
		ReadLockGuard(ReadLockGuard const&) = delete;
		ReadLockGuard(Lock &_lock);
		~ReadLockGuard();
	};

	class WriteLockGuard
	{
		Lock &srxw;
		std::unique_lock<std::mutex> lock;
	public:
		WriteLockGuard() = delete;
		WriteLockGuard(WriteLockGuard const&) = delete;
		WriteLockGuard(Lock &_lock);
		~WriteLockGuard();
	};
};

typedef SharedReadExclusiveWriteLock SRXWLock;
typedef SharedReadExclusiveWriteLock::ReadLockGuard SRXW_ReadLockGuard;
typedef SharedReadExclusiveWriteLock::WriteLockGuard SRXW_WriteLockGuard;
