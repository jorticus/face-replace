/******************************************************************/
/**
	@file	AudioStream.h
	@brief	Audio stream for kinect array mic
	@note	
	@todo
	@bug	
	@see	https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper

	@author	sadmb
	@date	Feb. 3, 2012
*/
/******************************************************************/
#ifndef KINECT_NUI_AUDIO_STREAM_H
#define KINECT_NUI_AUDIO_STREAM_H

#include <memory>

#include <Windows.h>
#include <wmcodecdsp.h>
#include <uuids.h> // FORMAT_WaveFormatEx and such
#include <mfapi.h> // IPropertyStore
#include <NuiApi.h>

#include "kinect/nui/StaticMediaBuffer.h"

#include "win32/Event.h"

namespace kinect {
	namespace nui {
		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		// implemented
		class AudioStream;
		// required
		class Kinect;

		typedef enum AEC_SYSTEM_MODE
			{	SINGLE_CHANNEL_AEC	= 0,
			ADAPTIVE_ARRAY_ONLY	= ( SINGLE_CHANNEL_AEC + 1 ) ,
			OPTIBEAM_ARRAY_ONLY	= ( ADAPTIVE_ARRAY_ONLY + 1 ) ,
			ADAPTIVE_ARRAY_AND_AEC	= ( OPTIBEAM_ARRAY_ONLY + 1 ) ,
			OPTIBEAM_ARRAY_AND_AEC	= ( ADAPTIVE_ARRAY_AND_AEC + 1 ) ,
			SINGLE_CHANNEL_NSAGC	= ( OPTIBEAM_ARRAY_AND_AEC + 1 ) ,
			MODE_NOT_SET	= ( SINGLE_CHANNEL_NSAGC + 1 ) 
			} 	AEC_SYSTEM_MODE;
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
		class AudioStream
		{
			friend class Kinect;

		public:
			~AudioStream();

			void Open(AEC_SYSTEM_MODE mode = OPTIBEAM_ARRAY_ONLY);

    		std::vector<BYTE> Read();
			WAVEFORMATEX GetWaveFormat();

			double GetAudioBeamAngle() const { return audioBeamAngle_; }
			double GetAudioAngle() const { return audioAngle_; }
			double GetAudioAngleConfidence() const { return audioAngleConfidence_; }

			bool Wait( DWORD dwMilliseconds = INFINITE ) { return event_.Wait( dwMilliseconds ); }
		private:
			AudioStream();
			AudioStream( const AudioStream& rhs );

			void CopySensor( INuiSensor* sensor );

		private:
			INuiSensor* sensor_;			///< kinect instance
			INuiAudioBeam* audioBeam_;
			IMediaObject* mediaObject_;
			IPropertyStore* propertyStore_;

			kinect::nui::StaticMediaBuffer outputMediaBuffer_;
			DMO_OUTPUT_DATA_BUFFER outputBufferStruct_;
			double audioBeamAngle_, audioAngle_, audioAngleConfidence_;

			HANDLE hStream_;								///< handle
			win32::Event    event_;							///< event handle
		};
	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_AUDIO_STREAM_H
