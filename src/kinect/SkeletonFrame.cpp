#include "kinect/nui/SkeletonFrame.h"
#include "kinect/nui/SkeletonEngine.h"

namespace kinect {
	namespace nui {
		//----------------------------------------------------------
		/**
			@biref	Constructor
		*/
		SkeletonFrame::SkeletonFrame( INuiSensor* sensor, DWORD dwMillisecondsToWait /*= 0*/ )
			: sensor_( sensor )
		{
			GetNextFrame( dwMillisecondsToWait );
		}

		//----------------------------------------------------------
		/**
			@biref	Destructor
		*/
		SkeletonFrame::~SkeletonFrame()
		{
		}

		//----------------------------------------------------------
		/**
			@biref	Transform smoothing data
		*/
		void SkeletonFrame::TransformSmooth( const NUI_TRANSFORM_SMOOTH_PARAMETERS *pSmoothingParams /*= 0*/ )
		{
			HRESULT ret = sensor_->NuiTransformSmooth( &skeletonFrame_, pSmoothingParams );
			if (FAILED(ret)) {
				// TODO fail announce
				return;
			}
		}

		//----------------------------------------------------------
		/**
			@biref	If skeleton is found
			@return	true when a skeleton founded
		*/
		bool SkeletonFrame::IsFoundSkeleton() const
		{
			for( int i = 0; i < NUI_SKELETON_COUNT; ++i ) {
				if( skeletonFrame_.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED) {
					return true;
				}
			}

			return false;
		}

		//----------------------------------------------------------
		/**
			@biref	If position is found
			@return	true when a position founded
		 */
		bool SkeletonFrame::IsFoundPosition() const
		{
			for( int i = 0; i < NUI_SKELETON_COUNT; ++i ) {
				if( skeletonFrame_.SkeletonData[i].eTrackingState == NUI_SKELETON_POSITION_TRACKED ||
					skeletonFrame_.SkeletonData[i].eTrackingState == NUI_SKELETON_POSITION_ONLY) {
					return true;
				}
			}

			return false;
		}
 
		//----------------------------------------------------------
		/**
			@biref	Get the next frame
		*/
		void SkeletonFrame::GetNextFrame( DWORD dwMillisecondsToWait /*= 0*/ )
		{
			HRESULT ret = sensor_->NuiSkeletonGetNextFrame( dwMillisecondsToWait, &skeletonFrame_ );
			if (FAILED(ret)) {
				// TODO fail announce
				return;
			}
		}
	} // namespace nui
} // namespace kinect
