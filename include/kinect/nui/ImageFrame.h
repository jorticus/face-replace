/******************************************************************/
/**
	@file	ImageFrame.h
	@brief	Image frame for kinect video/ depth camera
	@note	
	@todo
	@bug	
	@see	https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper

	@author	kaorun55
	@author	sadmb
	@date	Oct. 26, 2011 modified
*/
/******************************************************************/
#ifndef KINECT_NUI_IMAGE_FRAME_H
#define KINECT_NUI_IMAGE_FRAME_H

#include <memory>

#include <Windows.h>
#include <NuiApi.h>

namespace kinect {
	namespace nui {
		//////////////////////////////////////////////////////
		//				forward declarations				//
		//////////////////////////////////////////////////////
		// implemented
		class ImageFrame;
		class VideoFrame;
		class DepthFrame;
		// required
		class ImageStream;

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
			@class	ImageFrame
			@brief	Image frame for kinect video/ depth camera
			@note	
			@date	Oct. 26, 2011
		*/
		/****************************************/
		class ImageFrame
		{
		public:
			ImageFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait = 0 );

			virtual ~ImageFrame();

			INT Pitch(){ return lockedRect_.Pitch; };
			void* Bits() { return lockedRect_.pBits; };

			UINT Width() const;
			UINT Height() const;

		protected:

			ImageStream& imageStream_;			///< image stream corresponds

			NUI_IMAGE_FRAME imageFrame_;		///< Frame data
			NUI_LOCKED_RECT lockedRect_;	    ///< Rect data. Kinect SDK beta2 transition, Nov. 2, 2011
		};
		
		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
			@class	ImageFrame
			@brief	Image frame for kinect video/ depth camera
			@note	
			@date	Oct. 26, 2011
		*/
		/****************************************/
		class VideoFrame : public ImageFrame
		{
		public:
			VideoFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait = 0 );
			virtual ~VideoFrame();

			UINT operator () ( UINT x, UINT y );
		};

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
			@class	DepthFrame
			@brief	Depth frame for Kinect depth camera
			@note	
			@date	Oct. 26, 2011
		*/
		/****************************************/
		class DepthFrame : public ImageFrame
		{
		public:
			DepthFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait = 0 );
			virtual ~DepthFrame();

			USHORT operator () ( UINT x, UINT y );
		};
	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_IMAGE_FRAME_H
