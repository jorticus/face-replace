#include "kinect/nui/SkeletonEngine.h"

namespace kinect {
	namespace nui {
		//----------------------------------------------------------
		/**
			@brief	Constructor
		*/
		SkeletonEngine::SkeletonEngine()
			: sensor_(NULL)
			, event_( 0 )
			, isEnabled_( false )
		{
		}

		//----------------------------------------------------------
		/**
			@brief	Destructor
		*/
		SkeletonEngine::~SkeletonEngine()
		{
		}
		
		//----------------------------------------------------------
		/**
			@brief	Copy kinect instance
		*/
		void SkeletonEngine::CopySensor( INuiSensor* sensor )
		{
			if(sensor_ != NULL){
				sensor_->Release();
				sensor_ = NULL;
			}
			sensor_ = sensor;
		}

		//----------------------------------------------------------
		/**
			@brief	Enable the capturing of Skeleton data
		*/
		void SkeletonEngine::Enable( DWORD dwFlags /*= 0*/ )
		{
			HRESULT ret = sensor_->NuiSkeletonTrackingEnable( event_.get(), dwFlags );
			if (FAILED(ret)) {
				return;
			}

			isEnabled_ = true;
		}

		//----------------------------------------------------------
		/**
			@brief	Disable the capturing of Skeleton data
		*/
		void SkeletonEngine::Disable()
		{
			HRESULT ret = sensor_->NuiSkeletonTrackingDisable();
			if (FAILED(ret)) {
				return;
			}

			isEnabled_ = false;
		}
		
 		//----------------------------------------------------------
		/**
			@brief	Get the next frame
		*/
		SkeletonFrame SkeletonEngine::GetNextFrame( DWORD dwMillisecondsToWait /*= 0*/ )
		{

			return SkeletonFrame( sensor_, dwMillisecondsToWait );
		}
	} // namespace nui
} // namespace kinect