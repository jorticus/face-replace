/******************************************************************/
/**
	@file	Kinect.h
	@brief	Kinect wrapper class for cpp
	@note	
	@todo
	@bug	
	@see	https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper
	
	@author	kaorun55
	@author	sadmb
	@date	Oct. 26, 2011 modified
*/
/******************************************************************/
#ifndef KINECT_NUI_KINECT_H
#define KINECT_NUI_KINECT_H

#include <memory>
#include <map>

#include <Windows.h>
#include <NuiApi.h>

#include "kinect/nui/ImageStream.h"
#include "kinect/nui/SkeletonEngine.h"
#include "kinect/nui/AudioStream.h"
#include "kinect/nui/KinectContext.h"
#include "kinect/nui/KinectListener.h"

namespace kinect {
	namespace nui {
		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		// implemented
		class Kinect;

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
			@class	Kinect
			@brief	Kinect wrapper class for cpp
			@note	
			@date	Oct. 26, 2011
		*/
		/****************************************/
		class Kinect
		{
		public:

			Kinect(int index = -1);
			Kinect(BSTR deviceName);
			~Kinect();

			void Initialize( DWORD dwFlags );
			void Shutdown();

			bool IsInited();
			bool IsConnected();

			void WaitAndUpdateAll();

			void SetAngle( LONG lAngleDegrees );
			LONG GetAngle() const;

			static int GetActiveCount();
			static int GetAvailableCount();
			static int GetConnectedCount();
			static int GetNextAvailableIndex();
			
			int GetSensorIndex() const { return index_; }

			ImageStream& VideoStream() { return video_; }
			ImageStream& DepthStream() { return depth_; }
			kinect::nui::AudioStream& AudioStream() { return audio_; }
			SkeletonEngine& Skeleton() { return skeleton_; }

			LONG GetColorPixelCoordinatesFromDepthPixel(LONG lDepthIndex, USHORT usDepthValue);

			static const LONG CAMERA_ELEVATION_MAXIMUM = NUI_CAMERA_ELEVATION_MAXIMUM;	///< Maximum value of camera elevation 
			static const LONG CAMERA_ELEVATION_MINIMUM = NUI_CAMERA_ELEVATION_MINIMUM;	///< Minimum value of camera elevation

		private:
			friend class KinectContext;

			bool Connect(int index);
			bool Connect(BSTR deviceName);
			void Disconnect();
			void Cleanup();
			void StatusProc(HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName);

			

			ImageStream video_;				///< Video image from Kinect camera
			ImageStream depth_;				///< Depth image from Kinect depth sensor
			kinect::nui::AudioStream audio_;				///< Audio stream from Kinect array mic
			SkeletonEngine  skeleton_;		///< Skeleton Tracking
			BSTR uniqueId_;					///< Unique ID of this Kinect sensor
			int index_;						///< index of this Kinect sensor
			bool isConnected_;				///< is connected?
			bool isInited_;					///< is initialized?
			std::map<long, KinectListenerBase*> kinectListeners_;	///< listener when kinect plugged/ unplugged
		public:

            INuiSensor* sensor_;				///< sensor

			//////////////////////////////////////////////////////
			//				inline functions					//
			//////////////////////////////////////////////////////
			/**
				@brief	To add listener called when Kinect is plugged/ unplugged.
				@param	object				object which has functions
				@param	pluggedFunction		listener function pointer called when the kinect plugged
				@param	unpluggedFunction	listener function pointer called when the kinect unplugged
			*/
			template <class T> void AddKinectListener(T* object, void(T::* pluggedFunction)(), void(T::* unpluggedFunction)())
			{
				long key = reinterpret_cast<long>(object);
				kinectListeners_.insert(std::pair<long, KinectListenerBase*>(key, new KinectListener<T>(object, pluggedFunction, unpluggedFunction)));
			}

			/**
				@brief	To remove listener called when Kinect is plugged/ unplugged.
			*/
			template <class T> void RemoveKinectListener(T* object)
			{
				long key = reinterpret_cast<long>(object);
				std::map<long, KinectListenerBase*>::iterator it = kinectListeners_.find(key);
				kinectListeners_.erase(it);
			}
		};
	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_KINECT_H
