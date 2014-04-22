#include "kinect/nui/ImageStream.h"

namespace kinect {
	namespace nui {
		//----------------------------------------------------------
		/**
			@brief Constructor
		*/
        ImageStream::ImageStream()
            : sensor_( NULL )
            , hStream_( 0 )
            , event_( 0 )
			, nearMode_( false )
        {
        }

		//----------------------------------------------------------
		/**
			@brief Destructor
		*/
        ImageStream::~ImageStream()
        {
        }
		
		//----------------------------------------------------------
		/**
			@brief Copy instance
			@param	instance	pointer of NuiInstance
		*/
		void ImageStream::CopySensor( INuiSensor* sensor )
		{
			if(sensor_ != NULL){
				sensor_->Release();
				sensor_ = NULL;
			}
			sensor_ = sensor;
		}

		//----------------------------------------------------------
		/**
			@brief Open stream
		*/
		void ImageStream::Open( NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eResolution,
								bool nearMode /*= false */, DWORD dwImageFrameFlags_NotUsed /*= 0*/, DWORD dwFrameLimit /*= 2*/ )
		{
			HRESULT ret = sensor_->NuiImageStreamOpen( eImageType, eResolution, dwImageFrameFlags_NotUsed,
														dwFrameLimit, event_.get(), &hStream_ );
			resolution_ = eResolution;
			if (FAILED(ret)) {
				return;
			}
			if((eImageType != NUI_IMAGE_TYPE_DEPTH) && (eImageType != NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX)){
				nearMode = false;
			}
			nearMode_ = nearMode;
			if(nearMode_){
				ret = sensor_->NuiImageStreamSetImageFrameFlags(hStream_, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE);
				if (FAILED(ret)) {
					return;
				}
			}

            ::NuiImageResolutionToSize( eResolution, (DWORD&)width_, (DWORD&)height_ );
		}

		//----------------------------------------------------------
		/**
			@brief Get the next frame
		*/
		NUI_IMAGE_FRAME ImageStream::GetNextFrame( DWORD dwMillisecondsToWait /*= 0*/ )
		{
			NUI_IMAGE_FRAME imageFrame;
			HRESULT ret = sensor_->NuiImageStreamGetNextFrame( hStream_, dwMillisecondsToWait, &imageFrame );
			if (FAILED(ret)) {
				imageFrame.pFrameTexture = NULL;
			}

			return imageFrame;
		}

		//----------------------------------------------------------
		/**
			@brief Release the frame
		*/
        void ImageStream::ReleaseFrame(NUI_IMAGE_FRAME imageFrame )
        {
            HRESULT ret = sensor_->NuiImageStreamReleaseFrame( hStream_, &imageFrame );
			if (FAILED(ret)) {
				return;
			}
        }
	} // namespace nui
} // namespace kinect