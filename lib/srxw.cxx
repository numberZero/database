#include "srxw.hxx"

void SharedReadExclusiveWriteLock::read_lock()
{
	lock.lock();
	while(write_wait > 0)
		read_cv.wait(lock);
	++read_count;
	lock.unlock();
}

void SharedReadExclusiveWriteLock::read_unlock()
{
	lock.lock();
	--read_count;
	if((read_count == 0) && (write_wait > 0))
		write_cv.notify_one();
	lock.unlock();
}

void SharedReadExclusiveWriteLock::write_lock()
{
	lock.lock();
	++write_wait;
	while(read_count > 0)
		write_cv.wait(lock);
}

void SharedReadExclusiveWriteLock::write_unlock()
{
	--write_wait;
	if(write_wait > 0)
		write_cv.notify_one();
	else
		read_cv.notify_all();
	lock.unlock();
}

SRXW_ReadLockGuard::SRXW_ReadLockGuard(SRXWLock &_lock) :
	lock(_lock)
{
	lock.read_lock();
}

SRXW_ReadLockGuard::~SRXW_ReadLockGuard()
{
	lock.read_unlock();
}

SRXW_WriteLockGuard::SRXW_WriteLockGuard(SRXWLock &_lock) :
	lock(_lock)
{
	lock.write_lock();
}

SRXW_WriteLockGuard::~SRXW_WriteLockGuard()
{
	lock.write_unlock();
}
