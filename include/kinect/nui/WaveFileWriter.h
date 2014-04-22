/******************************************************************/
/**
	@file	WaveFileWriter.h
	@brief	Simple wave file writer
	@note	
	@todo
	@bug	
	@see	https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper
	
	@author	sadmb
	@date	Feb. 05, 2012
*/
/******************************************************************/
#ifndef KINECT_NUI_WAVE_FILE_WRITER_H
#define KINECT_NUI_WAVE_FILE_WRITER_H

#include <vector>
#include <Windows.h>
#include <mfapi.h>

namespace kinect {
	namespace nui {
		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		// implemented
		class WaveFileWriter;

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
			@class	WaveFileWriter
			@brief	Simple wave file writer
			@note	
			@date	Feb. 05, 2012
		*/
		/****************************************/
		class WaveFileWriter {
		public:
			WaveFileWriter() :
			  headerSize_( 0 ),
			  dataSize_( 0 ),
			  needFlush_( false )
			{

			}

			virtual ~WaveFileWriter()
			{
				Flush();
			}

			void Initialize(const TCHAR* outFile, int nSamplePerSec, int nBitPerSample, int nChannels)
			{
				hFile_ = CreateFile(
					outFile,
					GENERIC_WRITE,
					FILE_SHARE_READ,
					NULL,
					CREATE_ALWAYS,
					0,
					NULL
					);

				if(hFile_ == INVALID_HANDLE_VALUE){
					return;
				}

				wfxOut_.wFormatTag = 0x0001;	// PCM
				wfxOut_.nChannels = nChannels;
				wfxOut_.nSamplesPerSec = nSamplePerSec;
				wfxOut_.wBitsPerSample = nBitPerSample;
				wfxOut_.nBlockAlign = wfxOut_.wBitsPerSample * wfxOut_.nChannels / 8;
				wfxOut_.nAvgBytesPerSec = wfxOut_.nBlockAlign * wfxOut_.nSamplesPerSec;

				
				HRESULT hr = WriteWaveHeader(hFile_, &wfxOut_, &headerSize_);
			}

			void Write(std::vector<BYTE> buffer, DWORD cbProduced)
			{
				if(hFile_ == NULL){
					return;
				}

				WriteToFile(hFile_, buffer.data(), cbProduced);
				dataSize_ += cbProduced;

				needFlush_ = true;
			}

			void Flush()
			{
				if(needFlush_){
					FixUpChunkSizes(hFile_, headerSize_, dataSize_);
					needFlush_ = false;
				}
			}

			WAVEFORMATEX GetWaveFormat()
			{
				return wfxOut_;
			}

		private:
			HRESULT WriteWaveHeader(HANDLE hFile, WAVEFORMATEX *pWav, DWORD* pcbWritten)
			{
				HRESULT hr = S_OK;
				ULONG written = 0;
				UINT32 cbFormat = sizeof(WAVEFORMATEX);
				*pcbWritten = 0;

				DWORD header[] = {
					// RIFF header
					FCC('RIFF'),
					0,
					FCC('WAVE'),
					// Start of 'fmt ' chunk
					FCC('fmt '),
					cbFormat
				};

				DWORD dataHeader[] = { FCC('data'), 0 };

				hr = WriteToFile(hFile, header, sizeof(header));

				if(SUCCEEDED(hr))
				{
					hr = WriteToFile(hFile, pWav, cbFormat);
				}

				if(SUCCEEDED(hr))
				{
					hr = WriteToFile(hFile, dataHeader, sizeof(dataHeader));
				}

				if(SUCCEEDED(hr))
				{
					*pcbWritten = sizeof(header) + cbFormat + sizeof(dataHeader);
				}
				return hr;
			}

			HRESULT WriteToFile(HANDLE hFile, void* p, DWORD cb)
			{
				DWORD cbWritten = 0;
				HRESULT hr = S_OK;
				BOOL bResult = WriteFile(hFile, p, cb, &cbWritten, NULL);
				if(!bResult){
					hr = HRESULT_FROM_WIN32(GetLastError());
				}
				return hr;
			}

			HRESULT FixUpChunkSizes(HANDLE hFile, DWORD cbHeader, DWORD cbAudioData)
			{
				HRESULT hr = S_OK;

				LARGE_INTEGER li;
				li.QuadPart = cbHeader - sizeof(DWORD);

				if(0 == SetFilePointerEx(hFile, li, NULL, FILE_BEGIN)){
					hr = HRESULT_FROM_WIN32(GetLastError());
				}

				// Write the data size

				if(SUCCEEDED(hr)){
					hr = WriteToFile(hFile, &cbAudioData, sizeof(cbAudioData));
				}

				if(SUCCEEDED(hr)){
					// Write the file size
					li.QuadPart = sizeof(FOURCC);
					if(0 == SetFilePointerEx(hFile, li, NULL, FILE_BEGIN)){
						hr = HRESULT_FROM_WIN32(GetLastError());
					}
				}

				if(SUCCEEDED(hr)){
					DWORD cbRiffFileSize = cbHeader + cbAudioData - 8;

					// NOTE: The "size" field in the RIFF header does not include
					// the first 8 bytes of the file. i.e., it is the size of the
					// data that appears _after_ the size field.

					hr = WriteToFile(hFile, &cbRiffFileSize, sizeof(cbRiffFileSize));
				}
				return hr;
			}


		private:
			HANDLE hFile_;
			WAVEFORMATEX wfxOut_;
			ULONG headerSize_;
			ULONG dataSize_;
			bool needFlush_;
		};
	}
}

#endif // KINECT_NUI_WAVE_FILE_WRITER_H