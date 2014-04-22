#include "kinect/nui/ImageFrame.h"
#include "kinect/nui/ImageStream.h"

namespace kinect {
	namespace nui {
		//----------------------------------------------------------
        /**
			@brief	Constructor
		*/
        ImageFrame::ImageFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait /*= 0*/ )
            : imageStream_( imageStream )
            , imageFrame_( imageStream.GetNextFrame( dwMillisecondsToWait ) )
        {
            // Moved to VideoFrame class. Use that instead of ImageFrame
			/*if(imageFrame_.pFrameTexture != NULL){
	            imageFrame_.pFrameTexture->LockRect( 0, &lockedRect_, NULL, 0 );
			}*/
        }

		//----------------------------------------------------------
        /**
			@brief	Destructor
		*/
        ImageFrame::~ImageFrame()
        {
			if(imageFrame_.pFrameTexture != NULL){
				imageFrame_.pFrameTexture->UnlockRect(0);
			}
            imageStream_.ReleaseFrame( imageFrame_ );
        }
 
		//----------------------------------------------------------
		/**
			@brief	Get the width
		*/
        UINT ImageFrame::Width() const
        {
            NUI_SURFACE_DESC desc = { 0 };
			if(imageFrame_.pFrameTexture != NULL){
	            imageFrame_.pFrameTexture->GetLevelDesc( 0, &desc );
			}
            return desc.Width;
        }

		//----------------------------------------------------------
		/**
			@brief	Get the hight
		*/
        UINT ImageFrame::Height() const
        {
            NUI_SURFACE_DESC desc = { 0 };
			if(imageFrame_.pFrameTexture != NULL){
	            imageFrame_.pFrameTexture->GetLevelDesc( 0, &desc );
			}
            return desc.Height;
        }
 

		//----------------------------------------------------------
        /**
			@brief	Constructor
		*/
        VideoFrame::VideoFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait /*= 0*/ )
            : ImageFrame( imageStream, dwMillisecondsToWait )
        {
            if (imageFrame_.pFrameTexture != NULL){
                imageFrame_.pFrameTexture->LockRect(0, &lockedRect_, NULL, 0);
            }
        }

		//----------------------------------------------------------
        /**
			@brief	Destructor
		*/
        VideoFrame::~VideoFrame()
        {
        }

		//----------------------------------------------------------
		/**
			@brief	Data cordinated
		*/
        UINT VideoFrame::operator () ( UINT x, UINT y )
        {
            UINT* video = (UINT*)lockedRect_.pBits;
            return video[(Width() * y) + x];
        }

    
		//----------------------------------------------------------
        /**
			@brief	Constructor
		*/
        DepthFrame::DepthFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait /*= 0*/ )
            : ImageFrame( imageStream, dwMillisecondsToWait )
        {
            // Slightly better image depth (allows closer objects), also includes player index.
            // Image buffer is CV_16UC2 (channel 0: player index, channel 1: depth image)
            if (imageFrame_.pFrameTexture != NULL) {
                HRESULT ret;
                BOOL nearMode;
                INuiFrameTexture* pTexture;

                ret = imageStream_.sensor_->NuiImageFrameGetDepthImagePixelFrameTexture(
                    imageStream_.hStream_, &imageFrame_, &nearMode, &pTexture);

                if (FAILED(ret)) {
                    imageStream_.ReleaseFrame(imageFrame_);
                    return;
                }

                pTexture->LockRect(0, &lockedRect_, NULL, 0);
            }
        }

		//----------------------------------------------------------
        /**
			@brief	Destructor
		*/
        DepthFrame::~DepthFrame()
        {
        }

		//----------------------------------------------------------
		/**
			@brief	Data cordinated
		*/
        USHORT DepthFrame::operator () ( UINT x, UINT y )
        {
            USHORT* depth = (USHORT*)lockedRect_.pBits;
            UCHAR* d = (UCHAR*)&depth[(Width() * y) + x];
            return  (USHORT)(d[0] | d[1] << 8 );
        }
    } // namespace nui
} // namespace kinect