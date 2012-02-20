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


#include "VRPN/VRPNClient.h"

#include "Globals/Config.h"
#include "Geom/Point.h"
#include "Kinect/KinectSkeleton.h"
#include "VRPN/VRPNSkeletonTrackerRemote.h"
#include "VRPN/VRPNWiimoteRemote.h"

using namespace MultiKinect;
using namespace Geom;
using namespace Kinect;
using namespace VRPN;


VRPNClient::VRPNClient()
{
	nSkeletons_ = 0;
	skeletons_ = new KinectSkeleton[KINECT_SKELETON_COUNT];

	for (uint32 i = 0; i < KINECT_SKELETON_COUNT; i++)
	{
		if (Config::remoteVRPNSkeletons[i].enabled)
		{
			std::string trackerAddress = Config::remoteVRPNSkeletons[i].address + "@" + Config::remoteVRPNSkeletons[i].serverAddress;
			trackers_[i] = new VRPNSkeletonTrackerRemote(i, trackerAddress, this);
		}
		else trackers_[i] = 0;
	}

#ifdef _WIIMOTE_SUPPORT_
	for (uint32 i = 0; i < WIIMOTE_COUNT; i++)
	{
		if (Config::remoteVRPNWiimotes[i].enabled)
		{
			std::string moteAddress = Config::remoteVRPNWiimotes[i].address + "@" + Config::remoteVRPNWiimotes[i].serverAddress;
			wiimotes_[i] = new VRPNWiimoteRemote(i, moteAddress);
		}
		else wiimotes_[i] = 0;
	}
#endif

	processStopEvent_ = CreateEvent(0, true, false, 0);
	processThread_ = CreateThread(0, 0, processThread, this, 0, 0);
}

VRPNClient::~VRPNClient()
{
	if (processStopEvent_)
	{
		SetEvent(processStopEvent_);
		if (processThread_)
		{
			WaitForSingleObject(processThread_, INFINITE);
			CloseHandle(processThread_);
			processThread_ = 0;
		}
		CloseHandle(processStopEvent_);
		processStopEvent_ = 0;
	}

	for (uint32 i = 0; i < KINECT_SKELETON_COUNT; i++)
		if (trackers_[i]) delete trackers_[i];

#ifdef _WIIMOTE_SUPPORT_
	for (uint32 i = 0; i < WIIMOTE_COUNT; i++)
	{
		if (wiimotes_[1]) delete wiimotes_[i];
	}
#endif

	delete[] skeletons_;
}

void VRPNClient::notify(uint32 tracker, uint32 sensor, const float64* pos, const float64* quat)
{
	skeletons_[nSkeletons_].setPlayerIndex(tracker + 1);
	skeletons_[nSkeletons_].setJoint(
		basic_cast<KinectSkeleton::KinectJoint>(sensor),
		Point(
			basic_cast<float32>(pos[0]),
			basic_cast<float32>(pos[1]),
			basic_cast<float32>(pos[2])),
		Quaternion(
			basic_cast<float32>(quat[3]),
			basic_cast<float32>(quat[0]),
			basic_cast<float32>(quat[1]),
			basic_cast<float32>(quat[2])));
}

uint32 VRPNClient::getNumberOfSkeletons()
{
	return nSkeletons_;
}

KinectSkeleton* VRPNClient::getSkeletons()
{
	return skeletons_;
}

DWORD WINAPI VRPNClient::processThread(LPVOID param)
{
	VRPNClient* pThis = basic_cast<VRPNClient*>(param);
	pThis->processThread();
	return 0;
}

void VRPNClient::processThread()
{
	const uint32 nEvents = 1;
	HANDLE events[nEvents] = {processStopEvent_};

	bool exit = false;
	while(!exit)
	{
		nSkeletons_ = 0;
		for (uint32 i = 0; i < KINECT_SKELETON_COUNT; i++)
		{
			skeletons_[i].clear();
			if (trackers_[i])
			{
				trackers_[i]->mainloop();
				if (trackers_[i]->isValid()) nSkeletons_++;
			}
		}

#ifdef _WIIMOTE_SUPPORT_
		for (uint32 i = 0; i < WIIMOTE_COUNT; i++)
		{
			if (wiimotes_[i]) wiimotes_[i]->mainloop();
		}
#endif

		uint32 eventIndex = WaitForMultipleObjects(nEvents, events, false, 100);
		switch (eventIndex)
		{
		case WAIT_TIMEOUT:
			break;
		case WAIT_OBJECT_0:
			exit = true;
			break;
		default:
			break;
		}
	}
}
