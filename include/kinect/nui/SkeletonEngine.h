/******************************************************************/
/**
	@file	SkeletonEngine.h
	@brief	Skeleton engine for kinect
	@note	
	@todo
	@bug	
	@see		https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper

	@author	kaorun55
	@author	sadmb
	@date	Oct. 26, 2011 modified
 */
/******************************************************************/
#ifndef KINECT_NUI_SKELETON_ENGINE_H
#define KINECT_NUI_SKELETON_ENGINE_H

#include <memory>

#include <Windows.h>
#include <NuiApi.h>

#include "kinect/nui/SkeletonFrame.h"

#include "win32/Event.h"

namespace kinect {
	namespace nui {
		//////////////////////////////////////////////////////
		//				forward declaration					//
		//////////////////////////////////////////////////////
		// implemented
		class SkeletonEngine;
		// required
		class Kinect;

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
			@class	SkeletonEngine
			@brief	Skeleton engine for kinect
			@note	
			@date	Oct. 26, 2011
		 */
		/****************************************/
		class SkeletonEngine
		{
			friend class Kinect;

		public:
			~SkeletonEngine();

			void Enable( DWORD dwFlags = 0 );
			void Disable();

			SkeletonFrame GetNextFrame( DWORD dwMillisecondsToWait = 0 );

			bool IsEnabled() const { return isEnabled_; }
			bool Wait( DWORD dwMilliseconds = INFINITE ) { return event_.Wait( dwMilliseconds ); }

		private:

			SkeletonEngine();

			SkeletonEngine( const SkeletonEngine& rhs );

			void CopySensor ( INuiSensor* sensor );

		private:

			INuiSensor* sensor_; ///< pointer for kinect instance 
			win32::Event    event_; ///< event handle

			bool isEnabled_;
		};

	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_SKELETON_ENGINE_H
