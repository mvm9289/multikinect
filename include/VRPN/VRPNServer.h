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


#ifndef __VRPNSERVER_H__
#define __VRPNSERVER_H__

#include "Globals/Include.h"
#include <vrpn/vrpn_Connection.h>


namespace MultiKinect
{
	namespace VRPN
	{
		class VRPNServer
		{
		private:
			static bool initialized_;
			static vrpn_Connection_IP* conn_;
			static VRPNSkeletonTracker* trackers_[KINECT_SKELETON_COUNT];

#ifdef _WIIMOTE_SUPPORT_
			static VRPNWiimote* wiimotes_[WIIMOTE_COUNT];
#endif

			static HANDLE processStopEvent_;
			static HANDLE processThread_;
			static float32 vrpnFPS_;

		public:
			static void initialize();
			static void destroy();

			static bool isInitialized();
			static float32 getFPS();

			static DWORD WINAPI processThread(LPVOID param);
			static void processThread();
		};
	}
}

#endif
