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


#include "Render/RenderSystemLocal.h"

#include "Globals/Config.h"
#include "Globals/Vars.h"
#include "Geom/Matrix4x4.h"
#include "Kinect/KinectDevice.h"
#include "Kinect/KinectSkeleton.h"
#include "Tools/Log.h"

using namespace MultiKinect;
using namespace Geom;
using namespace Kinect;
using namespace Render;
using namespace Tools;

RenderSystemLocal::RenderSystemLocal(KinectDevice* device) : RenderSystem()
{
	device_ = device;

	int32 flags = 0;
	if (Config::kinect[Globals::INSTANCE_ID].rgbImage) flags = flags|KinectDevice::K_USE_COLOR;
	if (Config::kinect[Globals::INSTANCE_ID].skeletonTracking)
	{
		if (Config::kinect[Globals::INSTANCE_ID].depthMap) flags = flags|KinectDevice::K_USE_DEPTH_AND_PLAYER|KinectDevice::K_USE_SKELETON;
		else flags = flags|KinectDevice::K_USE_SKELETON;
	}
	else if (Config::kinect[Globals::INSTANCE_ID].depthMap) flags = flags|KinectDevice::K_USE_DEPTH;

	if (flags != 0)
	{
		uint32 initializeAttempts = 1;
		int32 result = device_->initialize(flags, Config::kinect[Globals::INSTANCE_ID].seatedMode, Config::kinect[Globals::INSTANCE_ID].nearMode);
		while (result != 0 && (initializeAttempts <= 1 || (result == KinectDevice::K_ERROR_DEVICE_ALREADY_INITIALIZED && initializeAttempts <= 2)))
		{
			initializeAttempts++;
			std::string message = "";
			switch (result)
			{
			case KinectDevice::K_ERROR_SKELETON_IN_USE:
				message = "ERROR: Unable to initialize RenderSystem. Skeleton is in use on device " + device_->getID() + ".";
				Log::write("[RenderSystem] initialize()", message);
				break;
			case KinectDevice::K_ERROR_SKELETON_TRACKING:
				message = "ERROR: Unable to enable skeleton tracking on device " + device_->getID() + ".";
				Log::write("[RenderSystem] initialize()", message);
				break;
			case KinectDevice::K_ERROR_DEVICE_IN_USE:
				message = "ERROR: Unable to initialize RenderSystem. Device " + device_->getID() + " is in use.";
				Log::write("[RenderSystem] initialize()", message);
				break;
			case KinectDevice::K_ERROR_INVALID_DEVICE:
				message = "ERROR: Unable to initialize RenderSystem. Device " + device_->getID() + " is not a valid device.";
				Log::write("[RenderSystem] initialize()", message);
				break;
			case KinectDevice::K_ERROR_DEVICE_ALREADY_INITIALIZED:
				message = "ERROR: Unable to initialize RenderSystem. Device " + device_->getID() + " is already initialized. Trying to reinitialize...";
				Log::write("[RenderSystem] initialize()", message);
				device_->shutDown();
				result = device_->initialize(flags, Config::kinect[Globals::INSTANCE_ID].seatedMode, Config::kinect[Globals::INSTANCE_ID].nearMode);
				break;
			case KinectDevice::K_ERROR_UNKNOWN:
				message = "ERROR: Unable to initialize RenderSystem. Device " + device_->getID() + " has produced an unknown error.";
				Log::write("[RenderSystem] initialize()", message);
				break;
			default:
				Log::write("[RenderSystem] initialize()", "ERROR: Unable to initialize RenderSystem. Unknown error.");
				break;
			}
		}
	}

	if (device_->isInitialized())
	{
		std::string message = "Initialized with Device " + device_->getID() + ".";
		Log::write("[RenderSystem] initialize()", message);
	}
}

RenderSystemLocal::~RenderSystemLocal()
{
	delete device_;
}

KinectDevice* RenderSystemLocal::getDevice()
{
	return device_;
}

uint8* RenderSystemLocal::getKColorFrame(int32 deviceIdx)
{
	return device_->getColorFrame();
}

uint8* RenderSystemLocal::getKDepthFrame(int32 deviceIdx)
{
	return device_->getDepthFrame();
}

void RenderSystemLocal::getKSkeletons(uint32& nSkeletons, KinectSkeleton*& skeletons, int32 deviceIdx)
{
	nSkeletons = device_->getNumberOfSkeletons();
	skeletons = device_->getSkeletons();
}

bool RenderSystemLocal::getKMatrix(Matrix4x4& kinectMatrix, int32 deviceIdx)
{
	float32 rotationX, rotationY, rotationZ;
	float32 translationX, translationY, translationZ;
	if (device_->getRotation(rotationX, rotationY, rotationZ) && device_->getTranslation(translationX, translationY, translationZ))
	{
		Matrix4x4 mRotationX = Matrix4x4().setRotation(rotationX, Matrix4x4::X_AXIS);
		Matrix4x4 mRotationY = Matrix4x4().setRotation(rotationY, Matrix4x4::Y_AXIS);
		Matrix4x4 mRotationZ = Matrix4x4().setRotation(rotationZ, Matrix4x4::Z_AXIS);
		Matrix4x4 mTranslation = Matrix4x4().setTranslation(Vector(translationX, translationY, translationZ));
		kinectMatrix = mTranslation*mRotationZ*mRotationY*mRotationX;
		return true;
	}
	else
	{
		kinectMatrix.setIdentity();
		return false;
	}
}

void RenderSystemLocal::getTransformedKSkeletons(uint32& nSkeletons, KinectSkeleton* skeletons, int32 deviceIdx)
{
	nSkeletons = device_->getNumberOfSkeletons();
	KinectSkeleton* originalSkeletons = device_->getSkeletons();
	Matrix4x4 kMatrix;
	getKMatrix(kMatrix, deviceIdx);
	for (uint32 i = 0; i < nSkeletons; i++)
	{
		skeletons[i] = originalSkeletons[i];
		for (uint32 j = 0; j < KINECT_SKELETON_JOINT_COUNT; j++)
		{
			KinectSkeleton::KinectJoint joint = basic_cast<KinectSkeleton::KinectJoint>(j);
			if (skeletons[i].getJointValidity(joint))
			{
				float32 psi, theta, phi;
				kMatrix.getEulerAngles(psi, theta, phi);
				Quaternion kQuaternion(phi, theta, psi);
				skeletons[i].setJoint(
					joint,
					kMatrix*skeletons[i].getJointPosition(joint),
					kQuaternion*skeletons[i].getJointOrientationQuaternion(joint));
			}
		}
	}
}
