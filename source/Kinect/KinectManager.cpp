/*
	MultiKinect: Skeleton tracking based on multiple Microsoft Kinect cameras
	Copyright (C) 2012-2013  Miguel Angel Vico Moya

	This file is part of MultiKinect.

	MultiKinect is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "Kinect/KinectManager.h"

#include "Kinect/KinectDevice.h"
#include "Tools/Log.h"
#include <Windows.h>
#include <NuiApi.h>


bool						KinectManager::initialized_	= false;
uint32						KinectManager::nDevices_	= 0;
std::vector<std::string>	KinectManager::devicesIDs_;


void KinectManager::initialize()
{
	if (!initialized_)
	{
		NuiSetDeviceStatusCallback(&deviceStatusCallback, 0);

		nDevices_ = 0;
		devicesIDs_.clear();

		int32 connectedDevices;
		if(FAILED(NuiGetSensorCount(&connectedDevices))) connectedDevices = 0;

		std::vector<std::string> connectedDevicesIDs;
		std::vector<std::string> connectedDevicesStatus;
		for (int32 i = 0; i < connectedDevices; i++)
		{
			KinectDevice deviceAux = KinectDevice(i);
			connectedDevicesIDs.push_back(deviceAux.getID());
			switch (deviceAux.getStatus())
			{
			case KinectDevice::K_DEVICE_OK:
				nDevices_++;
				devicesIDs_.push_back(deviceAux.getID());
				connectedDevicesStatus.push_back("OK");
				break;
			case KinectDevice::K_DEVICE_IN_USE:
				connectedDevicesStatus.push_back("In use");
				break;
			case KinectDevice::K_DEVICE_INITIALIZING:
				connectedDevicesStatus.push_back("Initializing");
				break;
			case KinectDevice::K_DEVICE_NOT_CONNECTED:
				connectedDevicesStatus.push_back("Not connected");
				break;
			case KinectDevice::K_DEVICE_NOT_VALID:
				connectedDevicesStatus.push_back("Not valid");
				break;
			case KinectDevice::K_DEVICE_UNSUPPORTED:
				connectedDevicesStatus.push_back("Unsupported");
				break;
			case KinectDevice::K_DEVICE_INSUFFICIENT_BANDWIDTH:
				connectedDevicesStatus.push_back("Insufficient bandwidth");
				break;
			case KinectDevice::K_DEVICE_NOT_POWERED:
				connectedDevicesStatus.push_back("Not powered");
				break;
			case KinectDevice::K_DEVICE_NOT_READY:
				connectedDevicesStatus.push_back("Not ready");
				break;
			default:
				connectedDevicesStatus.push_back("Unknown");
				break;
			}
		}

		Log::write("[KinectManager] initialize():");
		if (connectedDevices > 0)
		{
			Log::write("        Connected devices");
			for (int32 i = 0; i < connectedDevices; i++)
			{
				std::string text = "                ID: " + basic_cast<std::string>(i) + " (" + connectedDevicesIDs[i] + ")";
				Log::write(text, connectedDevicesStatus[i]);
			}

			if (nDevices_ > 0)
			{
				Log::write("        Available devices");
				for (uint32 i = 0; i < nDevices_; i++)
					Log::write("                ID: " + basic_cast<std::string>(i) + " (" + devicesIDs_[i] + ")");
			}
			else Log::write("        Available devices", 0);
		}
		else Log::write("        Connected devices", 0);

		initialized_ = true;
	}
	else Log::write("[KinectManager] initialize()", "ERROR: KinectManager already initialized.");
}

void KinectManager::destroy()
{
	if (initialized_)
	{
		nDevices_ = 0;
		devicesIDs_.clear();
		initialized_ = false;
	}
	else Log::write("[KinectManager] destroy()", "ERROR: KinectManager not initialized.");
}

bool KinectManager::isInitialized()
{
	return initialized_;
}

uint32 KinectManager::getNumberOfDevices()
{
	if (initialized_) return nDevices_;
	else
	{
		Log::write("[KinectManager] getNumberOfDevices()", "ERROR: KinectManager not initialized.");
		return 0;
	}
}

KinectDevice* KinectManager::getDevicePointer(uint32 index)
{
	if (initialized_)
	{
		if (index < nDevices_) return new KinectDevice(devicesIDs_[index]);
		else return 0;
	}
	else
	{
		Log::write("[KinectManager] getDevicePointer()", "ERROR: KinectManager not initialized.");
		return 0;
	}
}

KinectDevice* KinectManager::getDevicePointer(const std::string& deviceID)
{
	if (initialized_)
	{
		if (isValidDeviceID(deviceID)) return new KinectDevice(deviceID);
		else return 0;
	}
	else
	{
		Log::write("[KinectManager] getDevicePointer()", "ERROR: KinectManager not initialized.");
		return 0;
	}
}

KinectDevice KinectManager::getDeviceObject(uint32 index)
{
	if (initialized_)
	{
		if (index < nDevices_) return KinectDevice(devicesIDs_[index]);
		else return KinectDevice(-1);
	}
	else
	{
		Log::write("[KinectManager] getDeviceObject()", "ERROR: KinectManager not initialized.");
		return KinectDevice(-1);
	}
}

KinectDevice KinectManager::getDeviceObject(const std::string& deviceID)
{
	if (initialized_)
	{
		if (isValidDeviceID(deviceID)) return KinectDevice(deviceID);
		else return KinectDevice(-1);
	}
	else
	{
		Log::write("[KinectManager] getDeviceObject()", "ERROR: KinectManager not initialized.");
		return KinectDevice(-1);
	}
}

bool KinectManager::isValidDeviceID(const std::string& deviceID)
{
	if (initialized_)
	{
		bool found = false;
		for (uint32 i = 0; i < nDevices_ && !found; i++)
			if (devicesIDs_[i] == deviceID)
				found = true;
		return found;
	}
	else
	{
		Log::write("[KinectManager] isValidDeviceID()", "ERROR: KinectManager not initialized.");
		return false;
	}
}

std::string KinectManager::reformatDeviceID(const std::string& deviceID)
{
	std::string result = deviceID;
	for (int32 i = basic_cast<int32>(result.size()) - 1; i >= 0; i--)
	{
		if (result[i] == '\\' || result[i] == 38 /* == '&'*/)
			result.erase(i, 1);
	}
	return result;
}

void CALLBACK KinectManager::deviceStatusCallback(HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void* pUserData)
{
	Log::write("[KinectManager] deviceStatusCallback()", "Called!");
}
