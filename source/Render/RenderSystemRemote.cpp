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


#include "Render/RenderSystemRemote.h"

#include "Kinect/KinectSkeleton.h"
#include "Tools/Log.h"
#include "VRPN/VRPNClient.h"

using namespace MultiKinect;
using namespace Kinect;
using namespace Render;
using namespace Tools;
using namespace VRPN;

RenderSystemRemote::RenderSystemRemote() : RenderSystem()
{
	std::string message = "Initialized with VRPN Client.";
	Log::write("[RenderSystem] initialize()", message);

	vrpnClient_ = new VRPNClient();
}

RenderSystemRemote::~RenderSystemRemote()
{
	delete vrpnClient_;
}

uint8* RenderSystemRemote::getKColorFrame(int32 deviceIdx)
{
	return 0;
}

uint8* RenderSystemRemote::getKDepthFrame(int32 deviceIdx)
{
	return 0;
}

void RenderSystemRemote::getKSkeletons(uint32& nSkeletons, KinectSkeleton*& skeletons, int32 deviceIdx)
{
	nSkeletons = vrpnClient_->getNumberOfSkeletons();
	skeletons = vrpnClient_->getSkeletons();
}
