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


#include "VRPN/VRPNSkeletonTracker.h"

#include "Globals/Config.h"
#include "Geom/Point.h"
#include "Kinect/KinectManager.h"
#include "Kinect/KinectSkeleton.h"
#include "Render/RenderSystem.h"
#include "Tools/Log.h"
#include <vector>

using namespace MultiKinect;
using namespace Globals;
using namespace Geom;
using namespace Kinect;
using namespace Render;
using namespace Tools;
using namespace VRPN;


VRPNSkeletonTracker::VRPNSkeletonTracker(const std::string& name, vrpn_Connection* c) : vrpn_Tracker(name.c_str(), c)
{}

void VRPNSkeletonTracker::init(uint32 skeletonID, uint32 kinectID, const std::string& name)
{
	skeletonID_ = skeletonID;
	kinectID_ = kinectID;

	std::string trackerStr;
	if (kinectID_ > -1)
	{
		trackerStr = "Subtracker " + basic_cast<std::string>(skeletonID) + ":" + basic_cast<std::string>(kinectID_);
	}
	else
	{
		trackerStr = "Tracker " + basic_cast<std::string>(skeletonID);
	}

	std::string message = trackerStr + " initialized on " + name;
	Log::write("[VRPNSkeletonTracker] VRPNSkeletonTracker()", message);
}

VRPNSkeletonTracker::VRPNSkeletonTracker(uint32 skeletonID, const std::string& name, vrpn_Connection* c) : vrpn_Tracker(name.c_str(), c)
{
	init(skeletonID, -1, name);

	if (Config::system.currentMode == Config::KINECT_MASTER &&
		Config::localVRPNSkeletons[skeletonID_].sendOriginalSkeletons)
	{
		uint32 nDevices = KinectManager::getNumberOfDevices();
		for (uint32 i = 0; i < nDevices; i++)
		{
			std::string subName = name + "_" + basic_cast<std::string>(i);
			subtrackers_.push_back(new VRPNSkeletonTracker(subName, c));
			subtrackers_[i]->init(skeletonID, i, subName);
		}
	}
}

VRPNSkeletonTracker::~VRPNSkeletonTracker()
{
	for (uint32 i = 0; i < subtrackers_.size(); i++)
	{
		delete subtrackers_[i];
	}
	subtrackers_.clear();
}

void VRPNSkeletonTracker::mainloop()
{
	struct timeval _timestamp;
	vrpn_gettimeofday(&_timestamp, NULL);

	vrpn_Tracker::timestamp = _timestamp;

	KinectSkeleton skeleton;
	if (RenderSystem::isInitialized() && RenderSystem::getTransformedKinectSkeleton(skeleton, skeletonID_, kinectID_))
	{
		std::vector<Point> jointsPositions = skeleton.getJointsPositions();
		std::vector<Quaternion> jointsOrientations = skeleton.getJointsOrientationsQuaternions();
		std::vector<bool> validJoints = skeleton.getJointsValidity();
		for (uint32 j = 0; j < KINECT_SKELETON_JOINT_COUNT; j++)
		{
			if (validJoints[j])
			{
				d_sensor = j;

				pos[0] = jointsPositions[j].x;
				pos[1] = jointsPositions[j].y;
				pos[2] = jointsPositions[j].z;

				if (Config::localVRPNSkeletons[skeletonID_].sendOrientations)
				{
					d_quat[0] = jointsOrientations[j].imaginary().x;
					d_quat[1] = jointsOrientations[j].imaginary().y;
					d_quat[2] = jointsOrientations[j].imaginary().z;
					d_quat[3] = jointsOrientations[j].real();
				}
				else
				{
					d_quat[0] = 0.0f;
					d_quat[1] = 0.0f;
					d_quat[2] = 0.0f;
					d_quat[3] = 0.0f;
				}

				char buffer[1024];
				uint32 len = vrpn_Tracker::encode_to(buffer);
				if (d_connection->pack_message(len, _timestamp, position_m_id, d_sender_id, buffer, vrpn_CONNECTION_LOW_LATENCY))
				{
					Log::write("[VRPNSkeletonTracker] mainloop(): ", "Can not write message: Tossing");
				}
			}
		}
	}
	server_mainloop();

	for (uint32 i = 0; i < subtrackers_.size(); i++)
	{
		subtrackers_[i]->mainloop();
	}
}
