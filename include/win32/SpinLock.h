/******************************************************************/
/**
 * @file	SpinLock.h
 * @brief	Locker for multithread environment
 * @note	
 * @todo
 * @bug	
 * @see		https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper
 *
 * @author Naoto Nakamura
 * @author sadmb
 * @date Nov. 15, 2011
 */
/******************************************************************/
#ifndef KINECT_NUI_SPIN_LOCK
#define KINECT_NUI_SPIN_LOCK

#include <process.h>
#include <winbase.h>

namespace win32{
	//////////////////////////////////////////////////
	//				class declarations				//
	//////////////////////////////////////////////////
	/****************************************/
	/*!
		@class SpinLock
		@brief Locker class with no wait

		@author Naoto Nakamura
		@author sadmb
		@date Nov. 15, 2011
	*/
	/****************************************/
	class SpinLock
	{
	public:
		SpinLock()
			:mIsLocked(0){};

	public:

		bool tryLock()
		{
			long ret = InterlockedExchange(&mIsLocked, 1 );
			MemoryBarrier();
			return ret == 0;
		}

		virtual void lock()
		{
			while(!tryLock()){
				Sleep(0);
			}
		}
	
		virtual void unlock()
		{
			MemoryBarrier();
			*const_cast< long volatile* >( &mIsLocked ) = 0;
		}

		virtual bool isLocking(){return mIsLocked != 0;}

	private:
		long mIsLocked;

	};
} // namespace win32
#endif // KINECT_NUI_SPIN_LOCK