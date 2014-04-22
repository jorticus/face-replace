/******************************************************************/
/**
	@file	ImageStream.h
	@brief	Image stream for kinect video/ depth camera
	@note	
	@todo
	@bug	
	@see	https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper

	@author	kaorun55
	@author	sadmb
	@date	Oct. 26, 2011 modified
*/
/******************************************************************/
#ifndef KINECT_NUI_IMAGE_STREAM_H
#define KINECT_NUI_IMAGE_STREAM_H

#include <memory>

#include <Windows.h>
#include <NuiApi.h>

#include "win32/Event.h"

namespace kinect {
	namespace nui {
		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		// implemented
		class ImageStream;
		// required
		class Kinect;

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
			@class	ImageStream
			@brief	Image stream for kinect video/ depth camera
			@note	
			@date	Oct. 26, 2011
		*/
		/****************************************/
		class ImageStream
		{
			friend class Kinect;

		public:
			~ImageStream();

			void Open( NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eResolution,
								bool nearMode = false, DWORD dwImageFrameFlags_NotUsed = 0, DWORD dwFrameLimit = 2 );

    		NUI_IMAGE_FRAME GetNextFrame( DWORD dwMillisecondsToWait = 0 );
			void ReleaseFrame(NUI_IMAGE_FRAME imageFrame );

			bool Wait( DWORD dwMilliseconds = INFINITE ) { return event_.Wait( dwMilliseconds ); }

			bool IsNearMode() const { return nearMode_; }

			UINT Width() const { return width_; }
			UINT Height() const { return height_; }

			NUI_IMAGE_RESOLUTION Resolution() const { return resolution_; }

            HANDLE hStream_;								///< handle

            INuiSensor* sensor_;			///< kinect instance

		private:
			ImageStream();
			ImageStream( const ImageStream& rhs );

			void CopySensor( INuiSensor* sensor );

		private:
			
			
			win32::Event    event_;							///< event handle
			NUI_IMAGE_RESOLUTION resolution_;				///< resolution of this stream

			bool	nearMode_;									///< true if set nearmode

			UINT    width_;									///< width
			UINT    height_;								///< height
		};
	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_IMAGE_STREAM_H
