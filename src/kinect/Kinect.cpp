#include <vector>

#include "kinect/nui/Kinect.h"

namespace kinect {
	namespace nui {

		//----------------------------------------------------------
		/**
			@brief	Constructor
			@param	index	Index number of Kinect instance
			@arg		-1 when search next available device
		*/
		Kinect::Kinect(int index /* = -1*/)
		{
			sensor_ = NULL;
			Cleanup();
			if(index = -1){
				index = KinectContext::GetContext().GetNextAvailableIndex();
			}
			index_ = index;
			// add to the kinect context
			KinectContext::GetContext().Add(*this);
			// create instance if the kinect already plugged in
			Connect(index_);
		}

		//----------------------------------------------------------
		/**
			@brief	Constructor
			@param	deviceName	Device Name of the kinect sensor
			@details	To know the device name, open control panel, see property of Microsoft Kinect -> Microsoft Kinect Device, select Details tab, choose "Parent" property and it shows on Value.
		*/
		Kinect::Kinect(BSTR uniqueId)
		{
			sensor_ = NULL;
			Cleanup();
			uniqueId_ = uniqueId;
			// add to the kinect context
			KinectContext::GetContext().Add(*this);
			// create instance if the kinect already plugged in
			Connect(uniqueId);
		}

		//----------------------------------------------------------
		/**
			@brief	Destructor
		*/
		Kinect::~Kinect()
		{
			Disconnect();
			// remove from the kinect context
			KinectContext::GetContext().Remove(*this);
			// release kinect listener
			kinectListeners_.clear();
		}

		//----------------------------------------------------------
		/**
			@brief	Is connected?
			@return	true when connected
		*/
		bool Kinect::IsConnected()
		{
			return isConnected_;
		}

		//----------------------------------------------------------
		/**
			@brief	Is inited?
			@return	true when inited
		*/
		bool Kinect::IsInited()
		{
			return isInited_;
		}

		//----------------------------------------------------------
		/**
			@brief	Initialize Kinect
			@param	dwFlags Initializing Flags
			@arg		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX	Get depth data and player index
			@arg		NUI_INITIALIZE_FLAG_USES_COLOR					Get video data
			@arg		NUI_INITIALIZE_FLAG_USES_SKELETON				Get skeleton data
			@arg		NUI_INITIALIZE_FLAG_USES_DEPTH					Get depth data
		*/
		void Kinect::Initialize( DWORD dwFlags )
		{
			if(sensor_ == NULL){
				return;
			}
			HRESULT ret = sensor_->NuiInitialize( dwFlags );
			if (FAILED(ret)) {
				isInited_ = false;
				return;
			}
			isInited_ = true;
		}

		//----------------------------------------------------------
		/**
			@brief	Shutdown the kinect sensor
		*/
		void Kinect::Shutdown()
		{
			if(isConnected_){
				KinectContext::GetContext().Shutdown(*this);
			}
			isInited_ = false;
		}

		//----------------------------------------------------------
		/**
			@brief	Wait until all the kinect handler updated
		*/
		void Kinect::WaitAndUpdateAll()
		{
			std::vector< HANDLE >   handle;
			if ( VideoStream().hStream_ != NULL ) {
				handle.push_back( VideoStream().event_.get() );
			}

			if ( DepthStream().hStream_ != NULL ) {
				handle.push_back( DepthStream().event_.get() );
			}

			if( AudioStream().hStream_ != NULL ) {
				handle.push_back( AudioStream().event_.get() );
			}

			if ( Skeleton().IsEnabled() ) {
				handle.push_back( Skeleton().event_.get() );
			}

			if ( handle.size() == 0 ) {
				return;
			}

			DWORD ret = ::WaitForMultipleObjects( handle.size(), &handle[0], TRUE, INFINITE );
			if ( ret == WAIT_FAILED ) {
				return;
			}
		}

		//----------------------------------------------------------
		/**
			@brief	Set the angle of kinect tilt
			@param	lAngleDegrees	Target angle in degrees
		*/
		void Kinect::SetAngle( LONG lAngleDegrees )
		{
			// comment out here because i got an error.
			HRESULT ret = sensor_->NuiCameraElevationSetAngle( lAngleDegrees );
			//if ( ret != S_OK ) {
			//	throw win32::Win32Exception( ret );
			//}
		}

		//----------------------------------------------------------
		/**
			@brief	Get the current angle of kinect tilt
			@return	Current angle in degrees
		*/
		LONG Kinect::GetAngle() const
		{
			// comment out here because i got an error.
			LONG angle = 0;
			HRESULT ret = sensor_->NuiCameraElevationGetAngle( &angle );
			//if ( ret != S_OK ) {
			//	throw win32::Win32Exception( ret );
			//}

			return angle;
		}

		//----------------------------------------------------------
		/**
			@brief	Get the number of plugged kinect devices
			@return	The number of active kinects
		*/
		/*static*/ int Kinect::GetActiveCount()
		{
			return KinectContext::GetContext().GetActiveCount();
		}

		//----------------------------------------------------------
		/**
			@brief	Get the number of available kinect devices
			@return	The number of available kinects
		*/
		/*static*/ int Kinect::GetAvailableCount()
		{
			return KinectContext::GetContext().GetAvailableCount();
		}

		//----------------------------------------------------------
		/**
			@brief	Get the number of available kinect devices
			@return	The number of available kinects
		*/
		/*static*/ int Kinect::GetConnectedCount()
		{
			return KinectContext::GetContext().GetConnectedCount();
		}

		//----------------------------------------------------------
		/**
			@brief	Get the number of available kinect devices
			@return	The number of available kinects
		*/
		/*static*/ int Kinect::GetNextAvailableIndex()
		{
			return KinectContext::GetContext().GetNextAvailableIndex();
		}

		//----------------------------------------------------------
		/**
			@brief	Get color pixels coodinates from depth pixels
		*/
		LONG Kinect::GetColorPixelCoordinatesFromDepthPixel(LONG lDepthPixel, USHORT usDepthValue)
		{
			LONG lDepthX, lDepthY;
			lDepthX = (LONG)(lDepthPixel % DepthStream().Width());
			lDepthY = (LONG)(lDepthPixel / DepthStream().Width());
			LONG plColorX = 0, plColorY = 0;

			HRESULT ret = sensor_->NuiImageGetColorPixelCoordinatesFromDepthPixel(VideoStream().Resolution(), NULL, lDepthX, lDepthY, usDepthValue, &plColorX, &plColorY);
			if(FAILED(ret)) {
				return -1;
			}

			LONG plColorPixel = plColorY * VideoStream().Width() + plColorX;
			return plColorPixel;
		}

		//----------------------------------------------------------
		/**
			@brief	Connect the kinect
			@param	index	index for Kinect
		*/
		bool Kinect::Connect(int index)
		{
			if(sensor_ != NULL){
				sensor_->Release();
				sensor_ = NULL;
			}
			sensor_ = KinectContext::GetContext().Create(index);
			if(sensor_ == NULL){
				return false;
			}
			uniqueId_ = sensor_->NuiDeviceConnectionId();
			depth_.CopySensor(sensor_);
			video_.CopySensor(sensor_);
			skeleton_.CopySensor(sensor_);
			audio_.CopySensor(sensor_);
			isConnected_ = true;
			return true;
		}

		//----------------------------------------------------------
		/**
			@brief	Connect the kinect
			@param	deviceName	deviceName for Kinect
		*/
		bool Kinect::Connect(BSTR uniqueId)
		{
			if(sensor_ != NULL){
				sensor_->Release();
				sensor_ = NULL;
			}
			sensor_ = KinectContext::GetContext().Create(uniqueId);
			if(sensor_ == NULL){
				return false;
			}
			index_ = sensor_->NuiInstanceIndex();
			depth_.CopySensor(sensor_);
			video_.CopySensor(sensor_);
			skeleton_.CopySensor(sensor_);
			audio_.CopySensor(sensor_);
			isConnected_ = true;
			return true;
		}

		//----------------------------------------------------------
		/**
			@brief	Disconnect the kinect
		*/
		void Kinect::Disconnect()
		{
			Shutdown();
			Cleanup();
		}

		void Kinect::Cleanup()
		{
			index_ = -1;
			isConnected_ = false;
			isInited_ = false;
			uniqueId_ = NULL;
			if(sensor_ != NULL){
				sensor_->Release();
				sensor_ = NULL;
			}
		}

		//----------------------------------------------------------
		/**
			@brief	Callback when kinect state changed
		*/
		void Kinect::StatusProc(HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName)
		{
			if(SUCCEEDED(hrStatus)){
				std::map<long, KinectListenerBase*>::iterator it;
				for(it = kinectListeners_.begin(); it != kinectListeners_.end(); ++it){
					it->second->Plugged();
				}
			}else if(FAILED(hrStatus)){
				std::map<long, KinectListenerBase*>::iterator it;
				for(it = kinectListeners_.begin(); it != kinectListeners_.end(); ++it){
					it->second->Unplugged();
				}
			}
		}
	} ///< namespace nui
} ///< namespace kinect
