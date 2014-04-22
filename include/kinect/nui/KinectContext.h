/******************************************************************/
/**
	@file	KinectContext.h
	@brief	Global information about Kinect
	@note	Do not use or extend this class
	@todo
	@bug	
	@see	https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper

	@author	sadmb
	@date	Oct. 26, 2011
*/
/******************************************************************/
#ifndef KINECT_NUI_KINECT_CONTEXT_H
#define KINECT_NUI_KINECT_CONTEXT_H

#include <vector>

#include <Windows.h>
#include <NuiApi.h>

#include "kinect/nui/Kinect.h"
#include "win32/SpinLock.h"

namespace kinect {
	namespace nui {
		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		// implemented
		class KinectContext;

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
			@class	KinectContext
			@brief	Global information about Kinect
			@note	
			@date	Oct. 26, 2011
		*/
		/****************************************/
		class KinectContext
		{
		public:
			~KinectContext();

			static void CALLBACK KinectContext::StatusProcThunk(HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void* pUserData);
			void CALLBACK KinectContext::StatusProc(HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName);

			static KinectContext& GetContext();

			INuiSensor* Create(int index);
			INuiSensor* Create(const OLECHAR* strInstanceName);

			void Shutdown(Kinect& kinect);
			void ShutdownAll();

			void Add(Kinect& kinect);
			void Remove(Kinect& kinect);
			void RemoveAll();
			
			int GetActiveCount();
			int GetAvailableCount();
			int GetConnectedCount();
			int GetSensorIndex(Kinect& kinect);

			Kinect* getKinect( INuiSensor* sensor );
			
			bool IsConnected(int index);

			int GetNextAvailableIndex();

		private:
			KinectContext();

			std::vector<Kinect*> kinects_; ///< the connected kinect devices
			win32::SpinLock lock_;
		};
	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_KINECT_CONTEXT_H
