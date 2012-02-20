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


#include "VRPN/VRPNSkeletonTrackerRemote.h"

#include "Tools/Log.h"
#include "Tools/Timer.h"
#include "VRPN/VRPNClient.h"

using namespace MultiKinect;
using namespace Tools;
using namespace VRPN;


void VRPN_CALLBACK handle_tracker(void* userData, const vrpn_TRACKERCB t)
{
	VRPNSkeletonTrackerRemote* pThis = basic_cast<VRPNSkeletonTrackerRemote*>(userData);
	pThis->notify(t.sensor, t.pos, t.quat);
}

VRPNSkeletonTrackerRemote::VRPNSkeletonTrackerRemote(uint32 skeletonID, const std::string& name, VRPNClient* notifier) : vrpn_Tracker_Remote(name.c_str())
{
	skeletonID_ = skeletonID;
	notifier_ = notifier;
	register_change_handler(this, handle_tracker);
	timeoutId_ = "VRPNSkeletonTrackerTimeout" + basic_cast<std::string>(skeletonID_);
	Timer::startCount(timeoutId_);
	std::string message = "Tracker " + basic_cast<std::string>(skeletonID) + " initialized on " + name;
	Log::write("[VRPNSkeletonTrackerRemote] VRPNSkeletonTrackerRemote()", message);
}

VRPNSkeletonTrackerRemote::~VRPNSkeletonTrackerRemote()
{
	Timer::endCount(timeoutId_);
	unregister_change_handler(this, handle_tracker);
}

void VRPNSkeletonTrackerRemote::notify(uint32 sensor, const float64* pos, const float64* quat) const
{
	Timer::resetCount(timeoutId_);
	notifier_->notify(skeletonID_, sensor, pos, quat);
}

bool VRPNSkeletonTrackerRemote::isValid() const
{
	return Timer::getCountTime(timeoutId_) < 200;
}
