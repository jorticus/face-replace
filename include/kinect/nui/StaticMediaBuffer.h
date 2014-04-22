/******************************************************************/
/**
 * @file	StaticMediaBuffer.h
 * @brief	Kinect Audio wrapper class for cpp
 * @note	
 * @todo
 * @bug	
 * @see		https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper
 *
 * @author	kaorun55
 * @author	sadmb
 * @date	Jan. 20, 2012 modified
 */
/******************************************************************/
#ifndef KINECT_NUI_STATIC_MEDIA_BUFFER_H
#define KINECT_NUI_STATIC_MEDIA_BUFFER_H

#include <windows.h>
// For functions and definitions used to create output file
#include <dmo.h> // Mo*MediaType
#include <vector>

namespace kinect {
	namespace nui {
		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
		 * @class	StaticMediaBuffer
		 * @brief	Kinect Audio wrapper class for cpp
		 * @note	
		 * @date	Jan. 20, 2012
		 */
		/****************************************/
		class StaticMediaBuffer : public IMediaBuffer {
		public:
			StaticMediaBuffer() {}
			STDMETHODIMP_(ULONG) AddRef() { return 2; }
			STDMETHODIMP_(ULONG) Release() { return 1; }
			STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {
				if (riid == IID_IUnknown) {
					AddRef();
					*ppv = (IUnknown*)this;
					return NOERROR;
				}
				else if (riid == IID_IMediaBuffer) {
					AddRef();
					*ppv = (IMediaBuffer*)this;
					return NOERROR;
				}
				else
					return E_NOINTERFACE;
			}
			STDMETHODIMP SetLength(DWORD ulLength) {m_ulData = ulLength; return NOERROR;}
			STDMETHODIMP GetMaxLength(DWORD *pcbMaxLength) {*pcbMaxLength = buffer_.size(); return NOERROR;}
			STDMETHODIMP GetBufferAndLength(BYTE **ppBuffer, DWORD *pcbLength) {
				if (ppBuffer) *ppBuffer = &buffer_[0];
				if (pcbLength) *pcbLength = m_ulData;
				return NOERROR;
			}

			void Clear() {
				m_ulData = 0;
			}

			ULONG GetDataLength() const {
				return m_ulData;
			}

			void SetBufferLength(DWORD ulLength) {
				buffer_.resize(ulLength);
			}

			std::vector<BYTE> Clone() const {
				return std::vector<BYTE>(buffer_.begin(), buffer_.begin() + GetDataLength() );
			}
		protected:
			std::vector<BYTE> buffer_;
			ULONG m_ulData;
		};
	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_STATIC_MEDIA_BUFFER_H
