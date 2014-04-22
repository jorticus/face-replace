#include "kinect/nui/AudioStream.h"

namespace kinect {
	namespace nui {
		//----------------------------------------------------------
		/**
			@brief Constructor
		*/
        AudioStream::AudioStream()
            : sensor_( NULL )
			, audioBeam_( NULL )
			, mediaObject_( NULL )
			, propertyStore_( NULL )
            , hStream_( 0 )
            , event_( 0 )
        {
        }

		//----------------------------------------------------------
		/**
			@brief Destructor
		*/
        AudioStream::~AudioStream()
        {
			if(audioBeam_ != NULL){
				audioBeam_->Release();
				audioBeam_ = NULL;
			}

			if(mediaObject_ != NULL){
				mediaObject_->Release();
				mediaObject_ = NULL;
			}

			if(propertyStore_ != NULL){
				propertyStore_->Release();
				propertyStore_ = NULL;
			}
			CoUninitialize();
        }
		
		//----------------------------------------------------------
		/**
			@brief Copy instance
			@param	instance	pointer of NuiInstance
		*/
		void AudioStream::CopySensor( INuiSensor* sensor )
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
			@param	mode	AEC_SYSTEM_MODE
		*/
		void AudioStream::Open(AEC_SYSTEM_MODE mode /*= OPTIBEAM_ARRAY_ONLY */)
		{
			if((mode == ADAPTIVE_ARRAY_ONLY) || (mode == ADAPTIVE_ARRAY_AND_AEC) || (mode == MODE_NOT_SET)){
				return;
			}
			CoInitialize(NULL);
			HRESULT ret = sensor_->NuiGetAudioSource(&audioBeam_);
			if(FAILED(ret)){
				return;
			}
			ret = audioBeam_->QueryInterface(IID_IMediaObject, (void**)&mediaObject_);
			if(FAILED(ret)){
				return;
			}
			ret = audioBeam_->QueryInterface(IID_IPropertyStore, (void**)&propertyStore_);
			if(FAILED(ret)){
				return;
			}
			if(audioBeam_ != NULL){
				audioBeam_->Release();
				audioBeam_ = NULL;
			}

			PROPVARIANT pvSysMode;
			PropVariantInit(&pvSysMode);
			pvSysMode.vt = VT_I4;
			pvSysMode.lVal = (LONG)mode;
			ret = propertyStore_->SetValue(MFPKEY_WMAAECMA_SYSTEM_MODE, pvSysMode);
			if(FAILED(ret)){
				return;
			}
			PropVariantClear(&pvSysMode);

			DMO_MEDIA_TYPE mt = {0};
			// Set DMO output format
			ret = MoInitMediaType(&mt, sizeof(WAVEFORMATEX));
			if(FAILED(ret)){
				return;
			}
    
			mt.majortype = MEDIATYPE_Audio;
			mt.subtype = MEDIASUBTYPE_PCM;
			mt.lSampleSize = 0;
			mt.bFixedSizeSamples = TRUE;
			mt.bTemporalCompression = FALSE;
			mt.formattype = FORMAT_WaveFormatEx;	
			memcpy(mt.pbFormat, &GetWaveFormat(), sizeof(WAVEFORMATEX));
    
			ret = mediaObject_->SetOutputType(0, &mt, 0); 
			if(FAILED(ret)){
				return;
			}
			MoFreeMediaType(&mt);

			// Allocate streaming resources. This step is optional. If it is not called here, it
			// will be called when first time ProcessInput() is called. However, if you want to 
			// get the actual frame size being used, it should be called explicitly here.
			ret = mediaObject_->AllocateStreamingResources();
			if(FAILED(ret)){
				return;
			}
    
			// Get actually frame size being used in the DMO. (optional, do as you need)
			int iFrameSize;
			PROPVARIANT pvFrameSize;
			PropVariantInit(&pvFrameSize);
			propertyStore_->GetValue(MFPKEY_WMAAECMA_FEATR_FRAME_SIZE, &pvFrameSize);
			if(FAILED(ret)){
				return;
			}
			iFrameSize = pvFrameSize.lVal;
			PropVariantClear(&pvFrameSize);

			ret = mediaObject_->QueryInterface(IID_INuiAudioBeam, (void**)&audioBeam_);
			if(FAILED(ret)){
				return;
			}

			memset(&outputBufferStruct_, 0, sizeof(outputBufferStruct_));
			outputBufferStruct_.pBuffer = &outputMediaBuffer_;
			outputMediaBuffer_.SetBufferLength(GetWaveFormat().nSamplesPerSec * GetWaveFormat().nBlockAlign);
		}

		//----------------------------------------------------------
		/**
			@brief Get the format
		*/
		WAVEFORMATEX AudioStream::GetWaveFormat()
		{
			WAVEFORMATEX wfxOut = {WAVE_FORMAT_PCM, 1, 16000, 32000, 2, 16, 0};
			return wfxOut;
		}

		//----------------------------------------------------------
		/**
			@brief Get the next frame
		*/
		std::vector<BYTE> AudioStream::Read()
		{
			outputMediaBuffer_.Clear();
			do{
				outputBufferStruct_.dwStatus = 0;
				DWORD dwStatus;
				if(mediaObject_ == NULL){
					return std::vector<BYTE>(NULL);
				}
				HRESULT ret = mediaObject_->ProcessOutput(0, 1, &outputBufferStruct_, &dwStatus);
				if(FAILED(ret)){
					return std::vector<BYTE>(NULL);
				}
				ret = audioBeam_->GetBeam(&audioBeamAngle_);
				if(FAILED(ret)){
					return std::vector<BYTE>(NULL);
				}
				ret = audioBeam_->GetPosition(&audioAngle_, &audioAngleConfidence_);
				if(FAILED(ret)){
					return std::vector<BYTE>(NULL);
				}

			}while(outputBufferStruct_.dwStatus & DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE);

			return outputMediaBuffer_.Clone();
		}
	} // namespace nui
} // namespace kinect