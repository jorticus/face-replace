/******************************************************************/
/**
 * @file	KinectContext.cpp
 * @brief	Global information about Kinect.
 * @note	
 * @todo
 * @bug	
 * @see		https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper
 *
 * @author	sadmb
 * @date	Oct. 26, 2011
 */
/******************************************************************/
#include "kinect/nui/KinectContext.h"

namespace kinect {
	namespace nui {
		//----------------------------------------------------------
		/**
			@brief	Constructor
		*/
		KinectContext::KinectContext()
		{
			::NuiSetDeviceStatusCallback(&StatusProcThunk, this);
		}

		//----------------------------------------------------------
		/**
			@brief	Destructor
		*/
		KinectContext::~KinectContext()
		{
			ShutdownAll();
			RemoveAll();
		}

		//----------------------------------------------------------
		/**
			@brief	Callback function when kinect status changed.
		*/
		/*static*/ void CALLBACK KinectContext::StatusProcThunk(HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void* pUserData)
		{
			KinectContext::GetContext().StatusProc(hrStatus, instanceName, uniqueDeviceName);
		}
		
		//----------------------------------------------------------
		/**
			@brief	Callback function for this class
		*/
		void CALLBACK KinectContext::StatusProc(HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName)
		{
			int size;
			// create instance automatically (not init/ open stream yet)
			if(SUCCEEDED(hrStatus)){
				bool found = false;
				size = (int)kinects_.size();
				// if theres already a Kinect class corresponding to this kinect device.
				for(int i = 0; i < size; i++){
					if(kinects_[i]->uniqueId_ != NULL && 0 == wcscmp(kinects_[i]->uniqueId_, instanceName)){
						kinects_[i]->Connect(kinects_[i]->uniqueId_);
						std::wcout << "KINECT IS PLUGGED, Device Name:: " << instanceName << "\n" << std::endl;
						found = true;
						break;
					}
				}
				if(!found){
					int index = GetNextAvailableIndex();
					if(index >= 0){
						size = (int)kinects_.size();
						for(int i = 0; i < size; i++){
							if(!kinects_[i]->IsConnected()){
								kinects_[i]->index_ = index;
								kinects_[i]->Connect(index);
								std::wcout << "KINECT IS PLUGGED, Device Name:: " << instanceName << "\n" << std::endl;
								found = true;
								break;
							}
						}
					}
				}
			}

			size = (int)kinects_.size();
			for(int i = 0; i < size; i++){
				if(kinects_[i]->uniqueId_ != NULL && 0 == wcscmp(kinects_[i]->uniqueId_, instanceName)){
					kinects_[i]->StatusProc(hrStatus, instanceName, uniqueDeviceName);
					break;
				}
			}

			// shutdown automatically
			if(FAILED(hrStatus)){
				size = (int)kinects_.size();
				for(int i = 0; i < size; i++){
					if(kinects_[i]->uniqueId_ != NULL && 0 == wcscmp(kinects_[i]->uniqueId_, instanceName)){
						kinects_[i]->Disconnect();
						std::wcout<< "\n" << "KINECT IS UNPLUGGED, Device Name::  " << instanceName << "\n" << std::endl;
						break;
					}
				}
			}
		}
		
		//----------------------------------------------------------
		/**
			@brief	Get context (singleton)
		*/
		/*static*/ KinectContext& KinectContext::GetContext()
		{
			static KinectContext context;
			return context;
		}

		//----------------------------------------------------------
		/**
			@brief	Create kinect sensor
		*/
		INuiSensor* KinectContext::Create(int index)
		{
			INuiSensor* sensor = NULL;
			HRESULT ret = ::NuiCreateSensorByIndex(index, &sensor);
			if (SUCCEEDED(ret)) {
				lock_.lock();
				int size = (int)kinects_.size();
				for(int i = 0; i < size; i++){
					if(kinects_[i]->index_ == sensor->NuiInstanceIndex()){
						std::wcout << "Kinect[" << kinects_[i]->index_ << "] is connected, Device Name:: " << sensor->NuiDeviceConnectionId() << "\n" << std::endl;
						lock_.unlock();
						return sensor;
					}
				}
				lock_.unlock();
			}
			return NULL;
		}

		//----------------------------------------------------------
		/**
			@brief	Create kinect sensor
		*/
		INuiSensor* KinectContext::Create(const OLECHAR* strInstanceId)
		{
			INuiSensor* sensor = NULL;
			HRESULT ret = ::NuiCreateSensorById(strInstanceId, &sensor);
			if (SUCCEEDED(ret)) {
				lock_.lock();
				int size = (int)kinects_.size();
				for(int i = 0; i < size; i++){
					if(0 == wcscmp(kinects_[i]->uniqueId_, sensor->NuiDeviceConnectionId())){
						std::wcout << "Kinect[" << kinects_[i]->index_ << "] is connected, Device Name:: " << sensor->NuiDeviceConnectionId() << "\n" << std::endl;
						lock_.unlock();
						return sensor;
					}
				}
				lock_.unlock();
			}
			return NULL;
		}

		//----------------------------------------------------------
		/**
			@brief	Shutdown a kinect device
			@param	kinect	kinect object
		*/
		void KinectContext::Shutdown(Kinect& kinect)
		{
			if(IsConnected(kinect.index_)){
				lock_.lock();
				std::wcout << "Shutdown Kinect[" << kinect.index_ << "], Device Name:: " << kinect.uniqueId_ << "\n" << std::endl;
				kinect.sensor_->NuiShutdown();
				lock_.unlock();
			}
		}

		//----------------------------------------------------------
		/**
			@brief	Shutdown all currently connected kinect devices
		*/
		void KinectContext::ShutdownAll()
		{
			//lock_.lock();
			int size = (int)kinects_.size();
			for(int i = 0; i < size; i++){
				Shutdown(*kinects_[i]);
			}
			//lock_.unlock();
		}

		//----------------------------------------------------------
		/**
			@brief	Add a kinect object
			@param	kinect	kinect object	
		*/
		void KinectContext::Add(Kinect& kinect)
		{
			lock_.lock();
			kinects_.push_back(&kinect);
			lock_.unlock();
		}

		//----------------------------------------------------------
		/**
			@brief	Remove a kinect object
			@param	kinect	kinect object
		*/
		void KinectContext::Remove(Kinect& kinect)
		{
			lock_.lock();
			std::vector<Kinect*>::iterator it;
			if(kinects_.size() > 0){
				for(it = kinects_.begin(); it != kinects_.end(); ++it){
					Kinect* kn = *it;
					if(kn == &kinect){
						kinects_.erase(it);
						break;
					}
				}
			}
			lock_.unlock();
		}


		//----------------------------------------------------------
		/**
			@brief	Remove all currently added kinect objects
		*/
		void KinectContext::RemoveAll()
		{
			lock_.lock();
			kinects_.clear();
			lock_.unlock();
		}
		
		//----------------------------------------------------------
		/**
			@brief	Get the number of active Kinect sensor
			@return	number
		*/
		int KinectContext::GetActiveCount()
		{
			int count = 0;
			HRESULT ret = ::NuiGetSensorCount( &count );
			if (FAILED(ret)) {
				return 0;
			}

			return count;
		}

		//----------------------------------------------------------
		/**
			@brief	Get the number of available (active but not connected) Kinect sensor
			@return	number
		*/
		int KinectContext::GetAvailableCount()
		{
			return (GetActiveCount() - GetConnectedCount());
		}
		
		//----------------------------------------------------------
		/**
			@brief	Get the number of connected Kinect sensor
			@return	number
		*/
		int KinectContext::GetConnectedCount()
		{
			int count = 0;
			int size = (int)kinects_.size();
			for(int i = 0; i < size; i++){
				if(kinects_[i]->IsConnected()){
					count++;
				}
			}
			return count;
		}

		//----------------------------------------------------------
		/**
			@brief	Get the index of a Kinect instance
			@param	kinect	kinect object	
			@return	Instance index starts from 0
		*/
		int KinectContext::GetSensorIndex(Kinect& kinect)
		{
			int size = (int)kinects_.size();
			for(int i = 0; i < size; i++){
				if(&kinect == kinects_[i]){
					return kinects_[i]->GetSensorIndex();
				}
			}
			return -1;
		}

		//----------------------------------------------------------
		/**
			@brief	Get the Kinect object from a kinect instance pointer
			@return	NULL if not found
		*/
		Kinect* KinectContext::getKinect( INuiSensor* sensor )
		{
			int size = (int)kinects_.size();
			for(int i = 0; i < size; i++){
				if(sensor == kinects_[i]->sensor_){
					return kinects_[i];
				}
			}
			return NULL;
		}

		//----------------------------------------------------------
		/**
			@brief	Is the index already connected?
			@return	true when connected
		*/
		bool KinectContext::IsConnected(int index)
		{
			int size = (int)kinects_.size();
			for(int i = 0; i < size; i++){
				if(index == kinects_[i]->GetSensorIndex()){
					return kinects_[i]->IsConnected();
				}
			}
			return false;
		}

		//----------------------------------------------------------
		/**
			@brief	Get the index of next available Kinect device
			@return	-1 when no available
		*/
		int KinectContext::GetNextAvailableIndex()
		{
			int size = GetActiveCount();
			for(int i = 0; i < size; i++){
				if(!IsConnected(i)){
					return i;
				}
			}
			return -1;
		}
	} // namespace kinect
} // namespace nui

