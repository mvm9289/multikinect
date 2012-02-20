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


#include "Render/RenderSystem.h"

#include "Geom/Matrix4x4.h"
#include "Render/RenderSystemLocal.h"
#include "Render/RenderSystemRemote.h"
#include "Render/RenderSystemInterprocess.h"
#include "Kinect/KinectDevice.h"
#include "Kinect/KinectSkeleton.h"
#include "Tools/Log.h"

using namespace MultiKinect;
using namespace Geom;
using namespace Kinect;
using namespace Render;
using namespace Tools;


RenderSystem* RenderSystem::instance_ = 0;

void RenderSystem::initialize(RenderSystemMode mode, KinectDevice* device)
{
	if (!instance_)
	{
		switch (mode)
		{
		case RS_LOCAL_DEVICE:
			instance_ = new RenderSystemLocal(device);
			break;
		case RS_INTERPROCESS:
			instance_ = new RenderSystemInterprocess();
			break;
		case RS_REMOTE_DEVICE:
			instance_ = new RenderSystemRemote();
			break;
		default:
			break;
		}
	}
	else Log::write("[RenderSystem] initialize()", "ERROR: RenderSystem already initialized.");
}

void RenderSystem::destroy()
{
	if (instance_)
	{
		delete instance_;
		instance_ = 0;
	}
	else Log::write("[RenderSystem] destroy()", "ERROR: RenderSystem not initialized.");
}

bool RenderSystem::isInitialized()
{
	return (instance_ != 0);
}

bool RenderSystem::hasDevice()
{
	if (instance_) return (dynamic_cast<RenderSystemLocal*>(instance_) != 0);
	else
	{
		Log::write("[RenderSystem] hasDevice()", "ERROR: RenderSystem not initialized.");
		return false;
	}
}

KinectDevice* RenderSystem::getDevice()
{
	if (instance_)
	{
		RenderSystemLocal* aux = dynamic_cast<RenderSystemLocal*>(instance_);
		if (aux) return aux->getDevice();
		else return 0;
	}
	else
	{
		Log::write("[RenderSystem] getDevice()", "ERROR: RenderSystem not initialized.");
		return 0;
	}
}

uint8* RenderSystem::getKinectColorFrame(int32 deviceIdx)
{
	if (instance_) return instance_->getKColorFrame(deviceIdx);
	else
	{
		Log::write("[RenderSystem] getKinectColorFrame()", "ERROR: RenderSystem not initialized.");
		return 0;
	}
}

uint8* RenderSystem::getKinectDepthFrame(int32 deviceIdx)
{
	if (instance_) return instance_->getKDepthFrame(deviceIdx);
	else
	{
		Log::write("[RenderSystem] getKinectDepthFrame()", "ERROR: RenderSystem not initialized.");
		return 0;
	}
}

KinectSkeleton* RenderSystem::getKinectSkeleton(uint32 i, int32 deviceIdx)
{
	if (instance_)
	{
		uint32 nSkeletons = 0;
		KinectSkeleton* skeletons = 0;
		instance_->getKSkeletons(nSkeletons, skeletons, deviceIdx);
		if (i >= nSkeletons) return 0;
		return &skeletons[i];
	}
	else
	{
		Log::write("[RenderSystem] getKinectSkeleton()", "ERROR: RenderSystem not initialized.");
		return 0;
	}
}

void RenderSystem::getKinectSkeletons(uint32& nSkeletons, KinectSkeleton*& skeletons, int32 deviceIdx)
{
	if (instance_) instance_->getKSkeletons(nSkeletons, skeletons, deviceIdx);
	else
	{
		nSkeletons = 0;
		skeletons = 0;
		Log::write("[RenderSystem] getKinectSkeletons()", "ERROR: RenderSystem not initialized.");
	}
}

bool RenderSystem::getKinectMatrix(Matrix4x4& kinectMatrix, int32 deviceIdx)
{
	if (instance_) return instance_->getKMatrix(kinectMatrix, deviceIdx);
	else
	{
		Log::write("[RenderSystem] getKinectMatrix()", "ERROR: RenderSystem not initialized.");
		kinectMatrix.setIdentity();
		return false;
	}
}

bool RenderSystem::getTransformedKinectSkeleton(KinectSkeleton& skeleton, uint32 i, int32 deviceIdx)
{
	if (instance_)
	{
		uint32 nSkeletons = 0;
		KinectSkeleton skeletons[KINECT_SKELETON_COUNT];
		instance_->getTransformedKSkeletons(nSkeletons, skeletons, deviceIdx);
		if (i >= nSkeletons) return false;
		else
		{
			skeleton = skeletons[i];
			return true;
		}
	}
	else
	{
		Log::write("[RenderSystem] getTransformedKinectSkeleton()", "ERROR: RenderSystem not initialized.");
		return false;
	}
}

void RenderSystem::getTransformedKinectSkeletons(uint32& nSkeletons, KinectSkeleton* skeletons, int32 deviceIdx)
{
	if (instance_) instance_->getTransformedKSkeletons(nSkeletons, skeletons, deviceIdx);
	else
	{
		nSkeletons = 0;
		skeletons = 0;
		Log::write("[RenderSystem] getTransformedKinectSkeletons()", "ERROR: RenderSystem not initialized.");
	}
}

float32 RenderSystem::getKinectFPS()
{
	if (instance_)
	{
		KinectDevice* aux = getDevice();
		if (aux) return aux->getFPS();
		else return 0.0f;
	}
	else
	{
		Log::write("[RenderSystem] getKinectFPS()", "ERROR: RenderSystem not initialized.");
		return 0.0f;
	}
}

RenderSystem::RenderSystem()
{
}

RenderSystem::~RenderSystem()
{
}

bool RenderSystem::getKMatrix(Matrix4x4& kinectMatrix, int32 deviceIdx)
{
	kinectMatrix.setIdentity();
	return true;
}

void RenderSystem::getTransformedKSkeletons(uint32& nSkeletons, KinectSkeleton* skeletons, int32 deviceIdx)
{
	KinectSkeleton* originalSkeletons;
	getKSkeletons(nSkeletons, originalSkeletons, deviceIdx);
	for (uint32 i = 0; i < nSkeletons; i++) skeletons[i] = originalSkeletons[i];
}
