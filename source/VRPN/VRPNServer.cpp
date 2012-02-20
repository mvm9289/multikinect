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


#include "VRPN/VRPNServer.h"

#include "Globals/Definitions.h"
#include "Globals/Config.h"
#include "Tools/Log.h"
#include "Tools/Timer.h"
#include "VRPN/VRPNSkeletonTracker.h"
#include "VRPN/VRPNWiimote.h"
#include <sstream>

using namespace MultiKinect;
using namespace Tools;
using namespace VRPN;


bool VRPNServer::initialized_ = false;
vrpn_Connection_IP* VRPNServer::conn_ = 0;
VRPNSkeletonTracker* VRPNServer::trackers_[KINECT_SKELETON_COUNT];

#ifdef _WIIMOTE_SUPPORT_
VRPNWiimote* VRPNServer::wiimotes_[WIIMOTE_COUNT];
#endif

HANDLE VRPNServer::processStopEvent_ = 0;
HANDLE VRPNServer::processThread_ = 0;
float VRPNServer::vrpnFPS_ = 0.0f;

void VRPNServer::initialize()
{
	if (!initialized_)
	{
		conn_ = new vrpn_Connection_IP();
		for (uint32 i = 0; i < KINECT_SKELETON_COUNT; i++)
		{
			if (Config::localVRPNSkeletons[i].enabled)
				trackers_[i] = new VRPNSkeletonTracker(i, Config::localVRPNSkeletons[i].address, conn_);
			else trackers_[i] = 0;
		}

#ifdef _WIIMOTE_SUPPORT_
		for (uint32 i = 0; i < WIIMOTE_COUNT; i++)
		{
			if (Config::localVRPNWiimotes[i].enabled)
				wiimotes_[i] = new VRPNWiimote(i, Config::localVRPNWiimotes[i].address, conn_);
			else wiimotes_[i] = 0;
		}
#endif

		processStopEvent_ = CreateEvent(0, true, false, 0);
		processThread_ = CreateThread(0, 0, processThread, 0, 0, 0);
		initialized_ = true;
	}
	else Log::write("[VRPNServer] initialize()", "ERROR: VRPNServer already initialized.");
}

void VRPNServer::destroy()
{
	if (initialized_)
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

#ifdef _WIIMOTE_SUPPORT_
		for (uint32 i = 0; i < WIIMOTE_COUNT; i++)
			if (wiimotes_[i]) delete wiimotes_[i];
#endif

		for (uint32 i = 0; i < KINECT_SKELETON_COUNT; i++)
			if (trackers_[i]) delete trackers_[i];
		delete conn_;
		initialized_ = false;
	}
	else Log::write("[VRPNServer] destroy()", "ERROR: VRPNServer not initialized.");
}

bool VRPNServer::isInitialized()
{
	return initialized_;
}

float32 VRPNServer::getFPS()
{
	return vrpnFPS_;
}

DWORD WINAPI VRPNServer::processThread(LPVOID param)
{
	processThread();
	return 0;
}

void VRPNServer::processThread()
{
	const uint32 nEvents = 1;
	HANDLE events[nEvents] = {processStopEvent_};

	uint32 vrpnFrames = 0;
	vrpnFPS_ = 0.0f;
	Timer::startCount("vrpnFramerate");

	bool exit = false;
	while(!exit)
	{
		uint32 eventIndex = WaitForMultipleObjects(nEvents, events, false, 10);
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

		for (uint32 i = 0; i < KINECT_SKELETON_COUNT; i++)
			if (trackers_[i]) trackers_[i]->mainloop();

#ifdef _WIIMOTE_SUPPORT_
		for (uint32 i = 0; i < WIIMOTE_COUNT; i++)
			if (wiimotes_[i]) wiimotes_[i]->mainloop();
#endif

		conn_->mainloop();

		vrpnFrames++;
		if (Timer::getCountTime("vrpnFramerate") >= 1000)
		{
			vrpnFPS_ = basic_cast<float32>(vrpnFrames)*1000.0f/basic_cast<float32>(Timer::resetCount("vrpnFramerate"));
			vrpnFrames = 0;
		}
	}

	Timer::endCount("vrpnFramerate");
}
