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


#ifndef __KINECTMANAGER_H__
#define __KINECTMANAGER_H__

#include "Globals/Include.h"
#include <vector>
#include <Windows.h>


namespace MultiKinect
{
	namespace Kinect
	{
		class KinectManager
		{
		private:
			static bool						initialized_;
			static uint32					nDevices_;
			static std::vector<std::string>	devicesIDs_;

		public:
			static void initialize();
			static void destroy();
			
			static bool				isInitialized();
			static uint32			getNumberOfDevices();
			static KinectDevice*	getDevicePointer(uint32 index);
			static KinectDevice*	getDevicePointer(const std::string& deviceID);
			static KinectDevice		getDeviceObject(uint32 index);
			static KinectDevice		getDeviceObject(const std::string& deviceID);
			static bool				isValidDeviceID(const std::string& deviceID);
			static std::string		reformatDeviceID(const std::string& deviceID);

			static void CALLBACK deviceStatusCallback(HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void* pUserData);
		};
	}
}

#endif
