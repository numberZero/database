#include "srxw.hxx"

#ifndef USE_EXCLUSIVE_LOCK
#define USE_SRXW_LOCK
#endif

SharedReadExclusiveWriteLock::ReadLockGuard::ReadLockGuard(SRXWLock &_lock) :
	srxw(_lock),
	lock(_lock.mtx)
{
#ifdef USE_SRXW_LOCK
	while(srxw.write_wait > 0)
		srxw.read_cv.wait(lock);
	++srxw.read_count;
	lock.unlock();
#endif
}

SharedReadExclusiveWriteLock::ReadLockGuard::~ReadLockGuard()
{
#ifdef USE_SRXW_LOCK
	lock.lock();
	--srxw.read_count;
	if((srxw.read_count == 0) && (srxw.write_wait > 0))
		srxw.write_cv.notify_one();
#endif
}

SharedReadExclusiveWriteLock::WriteLockGuard::WriteLockGuard(SRXWLock &_lock) :
	srxw(_lock),
	lock(_lock.mtx)
{
#ifdef USE_SRXW_LOCK
	++srxw.write_wait;
	while(srxw.read_count > 0)
		srxw.write_cv.wait(lock);
#endif
}

SharedReadExclusiveWriteLock::WriteLockGuard::~WriteLockGuard()
{
#ifdef USE_SRXW_LOCK
	--srxw.write_wait;
	if(srxw.write_wait > 0)
		srxw.write_cv.notify_one();
	else
		srxw.read_cv.notify_all();
#endif
}
