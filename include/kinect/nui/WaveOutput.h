/******************************************************************/
/**
	@file	WaveOutput.h
	@brief	Simple output wave audio
	@note	
	@todo
	@bug	
	@see	https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper
	
	@author	kaorun55
	@author	sadmb
	@date	Feb. 03, 2012 modified
*/
/******************************************************************/
#ifndef KINECT_NUI_WAVE_OUTPUT_H
#define KINECT_NUI_WAVE_OUTPUT_H

#include <vector>

#include <Windows.h>
#include <MMSystem.h>

namespace kinect {
	namespace nui {
		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		// implemented
		class WaveOutput;

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
			@class	WaveOutput
			@brief	Simple wave output
			@note	
			@date	Feb. 03, 2012
		*/
		/****************************************/
		class WaveOutput {
		public:
			WaveOutput()
			{
			}

			virtual ~WaveOutput()
			{
				waveOutPause(hWaveOut);

				for(size_t i = 0; i < waveHeaders_.size(); ++i){
					delete[] waveHeaders_[i].lpData;
				}
			}

			void Initialize(int nSamplePerSec, int nBitPerSample, int nChannels, int audioBufferCount = 100)
			{
				audioBufferCount_ = audioBufferCount;
				audioBufferNextIndex_ = 0;

				WAVEFORMATEX wfx;
				wfx.wFormatTag = 0x0001;	// PCM
				wfx.nChannels = nChannels;
				wfx.nSamplesPerSec = nSamplePerSec;
				wfx.wBitsPerSample = nBitPerSample;
				wfx.nBlockAlign = wfx.wBitsPerSample * wfx.nChannels / 8;
				wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;


				MMRESULT mmRes = ::waveOutOpen( &hWaveOut, WAVE_MAPPER, &wfx, NULL, NULL, CALLBACK_NULL);
				if(mmRes != MMSYSERR_NOERROR){
					throw std::runtime_error("waveOutOpen failed\n");
				}

				waveHeaders_.resize(audioBufferCount_);
				memset(&waveHeaders_[0], 0, sizeof(WAVEHDR) * waveHeaders_.size());

				for(size_t i = 0; i < waveHeaders_.size(); ++i){
					waveHeaders_[i].lpData = new char[MAX_BUFFER_SIZE];
					waveHeaders_[i].dwUser = i;
					waveHeaders_[i].dwFlags = WHDR_DONE;
				}
			}

			void Output( const void* buffer, DWORD length )
			{
				WAVEHDR* pHeader = &waveHeaders_[audioBufferNextIndex_];
				if((pHeader->dwFlags & WHDR_DONE) == 0){
					return;
				}

				// clean up the wave header
				MMRESULT mmRes = ::waveOutUnprepareHeader(hWaveOut, pHeader, sizeof(WAVEHDR));

				if(mmRes != MMSYSERR_NOERROR){
					return;
				}

				// Get the wave data
				pHeader->dwBufferLength = length;
				pHeader->dwFlags = 0;
				memcpy(pHeader->lpData, buffer, pHeader->dwBufferLength);

				// Initialize wave header
				mmRes = ::waveOutPrepareHeader(hWaveOut, pHeader, sizeof(WAVEHDR));
				if(mmRes != MMSYSERR_NOERROR){
					return;
				}

				// Put wave data to output queue
				mmRes = ::waveOutWrite(hWaveOut, pHeader, sizeof(WAVEHDR));
				if(mmRes != MMSYSERR_NOERROR){
					return;
				}

				// next buffer index
				audioBufferNextIndex_ = (audioBufferNextIndex_ + 1) % audioBufferCount_;
			}


		private:
			WaveOutput( const WaveOutput& rhs );
			WaveOutput& operator = ( const WaveOutput& rhs );

		private:
			static const int MAX_BUFFER_SIZE = 2 * 1024 * 1024;

			HWAVEOUT hWaveOut;
			std::vector<WAVEHDR> waveHeaders_;

			int audioBufferCount_;
			int audioBufferNextIndex_;
		};
	} // nui
} // kinect

#endif // KINECT_NUI_WAVE_OUTPUT_H
