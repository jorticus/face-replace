/******************************************************************/
/**
 * @file	KinectListener.h
 * @brief	listener class for plugging/ unplugging kinect.
 * @note	
 * @todo
 * @bug	
 * @see		https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper
 *
 * @author	sadmb
 * @date	Nov. 21, 2011
 */
/******************************************************************/
#ifndef KINECT_NUI_KINECT_LISTENER_H
#define KINECT_NUI_KINECT_LISTENER_H

#include <iostream>

namespace kinect{
	namespace nui{
		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
		 * @class	KinectListenerBody
		 * @brief	listener class for plugging/ unplugging kinect
		 * @note	
		 * @date	Nov. 21, 2011
		 */
		/****************************************/
		class KinectListenerBase {
		public:
			/**
			 * @brief	Destructor
			 */
			virtual ~KinectListenerBase(){}

			/**
			 * @brief	plugged event
			 */
			virtual void Plugged() = 0;

			/**
			 * @brief	unplugged event
			 */
			virtual void Unplugged() = 0;
		};

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
		 * @class	KinectListener
		 * @brief	listener class for plugging/ unplugging kinect
		 * @note	
		 * @date	Nov. 21, 2011
		 */
		/****************************************/
		template <class T>
		class KinectListener : public KinectListenerBase {
		public:
			/**
			 * @brief Constructor
			 */
			KinectListener(T* object, void(T::*pluggedFunction)() = NULL, void(T::*unpluggedFunction)() = NULL){
				mObject = object;
				mPluggedFunction = pluggedFunction;
				mUnpluggedFunction = unpluggedFunction;
			}

			/**
			 * @brief	Destructor
			 */
			virtual ~KinectListener(){}

			/**
			 * @brief	plugged event
			 */
			virtual void Plugged()
			{
				if(mPluggedFunction != NULL){
					(mObject->*mPluggedFunction)();
				}
			}

			/**
			 * @brief	unplugged event
			 */
			virtual void Unplugged()
			{
				if(mUnpluggedFunction != NULL){
					(mObject->*mUnpluggedFunction)();
				}
			}
		private:
			T* mObject;
			void (T::*mPluggedFunction)();
			void (T::*mUnpluggedFunction)();
		};
	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_KINECTLISTENER_H
