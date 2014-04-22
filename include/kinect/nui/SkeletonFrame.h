/******************************************************************/
/**
	@file	SkeletonFrame.h
	@brief	Skeleton frame and skeleton data for kinect video/ depth camera
	@note	
	@todo
	@bug	
	@see		https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper

	@author	kaorun55
	@author	sadmb
	@date	Oct. 26, 2011 modified
*/
/******************************************************************/
#ifndef KINECT_NUI_SKELETON_FRAME_H
#define KINECT_NUI_SKELETON_FRAME_H

#include <memory>
#include <tuple>

#include <Windows.h>
#include <NuiApi.h>

namespace kinect {
	namespace nui {
		//////////////////////////////////////////////////////
		//				forward declaration					//
		//////////////////////////////////////////////////////
		// implemented
		class SkeletonFrame;
		class SkeletonData;

		// required
		class SkeletonEngine;


		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
			@class	SkeletonData
			@brief	Skeleton data for kinect video/ depth camera
			@note	
			@date	Oct. 26, 2011
		*/
		/****************************************/
		class SkeletonData
		{
			friend class SkeletonFrame;

		public:
			struct SkeletonPoint {
				LONG   x;		///<	X position of skeleton joint
				LONG   y;		///<	Y position of skeleton joint
				USHORT  depth;	///<	depth data of skeleton joint

				SkeletonPoint() : x( 0 ), y( 0 ), depth( 0 ) {}
			};

			static const int POSITION_COUNT = NUI_SKELETON_POSITION_COUNT; ///< number of skeleton joints

			//////////////////////////////////////////////////////
			//				inline implementation				//
			//////////////////////////////////////////////////////
			//----------------------------------------------------------
			/**
				@brief transform skeleton data to Point struct
			*/
			SkeletonPoint TransformSkeletonToDepthImage( int index )
			{
				SkeletonPoint p;
				NuiTransformSkeletonToDepthImage( skeletonData_.SkeletonPositions[index], &p.x, &p.y, &p.depth );
				return p;
			}

			//----------------------------------------------------------
			/**
				@brief Get tracking state
			*/
			NUI_SKELETON_TRACKING_STATE TrackingState() const { return skeletonData_.eTrackingState; }

			//----------------------------------------------------------
			Vector4 operator [] ( int index )
			{
				return skeletonData_.SkeletonPositions[index];
			}

			//----------------------------------------------------------
			/**
				@brief Get user index
			*/
			DWORD GetUserIndex() const { return skeletonData_.dwUserIndex; }

		private:
			//----------------------------------------------------------
			/**
				@brief Constructor
			*/
			SkeletonData( NUI_SKELETON_DATA& skeletonData )
				: skeletonData_( skeletonData )
			{
			}

		private:

			NUI_SKELETON_DATA&   skeletonData_; ///< skeleton data
		};

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
			@class	SkeletonFrame
			@brief	Skeleton frame for kinect video/ depth camera
			@note	
			@date	Nov. 21, 2011
		*/
		/****************************************/
		class SkeletonFrame
		{
			friend class SkeletonEngine;

		public:

			static const int SKELETON_COUNT = NUI_SKELETON_COUNT; ///< number of skeletons
			virtual ~SkeletonFrame();

			void TransformSmooth( const NUI_TRANSFORM_SMOOTH_PARAMETERS *pSmoothingParams = 0 );

			bool IsFoundSkeleton() const;
			bool IsFoundPosition() const;

			SkeletonData operator [] ( int index ) { return SkeletonData( skeletonFrame_.SkeletonData[index] ); }

		private:
			SkeletonFrame( INuiSensor* sensor, DWORD dwMillisecondsToWait = 0 );

			void GetNextFrame( DWORD dwMillisecondsToWait = 0 );

		protected:

			INuiSensor* sensor_; ///< kinect sensor

			NUI_SKELETON_FRAME skeletonFrame_; ///< skeleton frame
		};
	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_SKELETON_FRAME_H
