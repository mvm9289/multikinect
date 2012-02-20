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


#include "Render/RenderSystemInterprocess.h"

#include "Globals/Config.h"
#include "Geom/Matrix4x4.h"
#include "Interprocess/SharedMemoryManager.h"
#include "Kinect/KinectDevice.h"
#include "Kinect/KinectManager.h"
#include "Kinect/KinectSkeleton.h"
#include "Tools/Log.h"

using namespace MultiKinect;
using namespace Geom;
using namespace Interprocess;
using namespace Kinect;
using namespace Render;
using namespace Tools;


RenderSystemInterprocess::RenderSystemInterprocess() : RenderSystem()
{
	currentSharedSegment_ = "";
	Log::write("[RenderSystem] initialize()", "Interprocess initialized");
}

RenderSystemInterprocess::~RenderSystemInterprocess()
{
}

void RenderSystemInterprocess::searchBestSharedSegment()
{
	float32* confidenceValue = 0;
	if (currentSharedSegment_ != "")
		confidenceValue = SharedMemoryManager::getSharedObject<float32>(currentSharedSegment_, "confidenceValue");

	uint32 nDevices = KinectManager::getNumberOfDevices();
	if (nDevices)
	{
		float32 confidenceMax = -1.0f;
		if (confidenceValue) confidenceMax = *confidenceValue + Config::other.confidenceMargin;
		for (uint32 i = 0; i < nDevices; i++)
		{
			std::string deviceID = KinectManager::getDeviceObject(i).getID();
			std::string segmentID = KinectManager::reformatDeviceID(deviceID);
			confidenceValue = SharedMemoryManager::getSharedObject<float32>(segmentID, "confidenceValue");
			if (confidenceValue && *confidenceValue > confidenceMax)
			{
				currentSharedSegment_ = segmentID;
				confidenceMax = *confidenceValue;
			}
		}
	}
	else currentSharedSegment_ = "";
}

uint8* RenderSystemInterprocess::getKColorFrame(int32 deviceIdx)
{
	std::string segmentID = "";

	if (deviceIdx == -1)
	{
		searchBestSharedSegment();
		segmentID = currentSharedSegment_;
	}
	else
	{
		std::string deviceID = KinectManager::getDeviceObject(deviceIdx).getID();
		segmentID = KinectManager::reformatDeviceID(deviceID);
	}

	if (segmentID != "") return SharedMemoryManager::getSharedObject<uint8>(segmentID, "colorFrame");
	else return 0;
}

uint8* RenderSystemInterprocess::getKDepthFrame(int32 deviceIdx)
{
	std::string segmentID = "";

	if (deviceIdx == -1)
	{
		searchBestSharedSegment();
		segmentID = currentSharedSegment_;
	}
	else
	{
		std::string deviceID = KinectManager::getDeviceObject(deviceIdx).getID();
		segmentID = KinectManager::reformatDeviceID(deviceID);
	}

	if (segmentID != "") return SharedMemoryManager::getSharedObject<uint8>(segmentID, "depthFrame");
	else return 0;
}

void RenderSystemInterprocess::getKSkeletons(uint32& nSkeletons, KinectSkeleton*& skeletons, int32 deviceIdx)
{
	std::string segmentID = "";

	if (deviceIdx == -1)
	{
		searchBestSharedSegment();
		segmentID = currentSharedSegment_;
	}
	else
	{
		std::string deviceID = KinectManager::getDeviceObject(deviceIdx).getID();
		segmentID = KinectManager::reformatDeviceID(deviceID);
	}

	if (segmentID != "")
	{
		uint32* aux = SharedMemoryManager::getSharedObject<uint32>(segmentID, "nSkeletons");
		if (aux)
		{
			nSkeletons = *aux;
			skeletons = SharedMemoryManager::getSharedObject<KinectSkeleton>(segmentID, "skeletons");
		}
		else
		{
			nSkeletons = 0;
			skeletons = 0;
		}
	}
	else
	{
		nSkeletons = 0;
		skeletons = 0;
	}
}

bool RenderSystemInterprocess::getKMatrix(Matrix4x4& kinectMatrix, int32 deviceIdx)
{
	std::string segmentID = "";

	if (deviceIdx == -1)
	{
		searchBestSharedSegment();
		segmentID = currentSharedSegment_;
	}
	else
	{
		std::string deviceID = KinectManager::getDeviceObject(deviceIdx).getID();
		segmentID = KinectManager::reformatDeviceID(deviceID);
	}

	float32* rotationX = 0;
	float32* rotationY = 0;
	float32* rotationZ = 0;
	float32* translationX = 0;
	float32* translationY = 0;
	float32* translationZ = 0;

	if (segmentID != "")
	{
		rotationX = SharedMemoryManager::getSharedObject<float32>(segmentID, "rotationX");
		rotationY = SharedMemoryManager::getSharedObject<float32>(segmentID, "rotationY");
		rotationZ = SharedMemoryManager::getSharedObject<float32>(segmentID, "rotationZ");
		translationX = SharedMemoryManager::getSharedObject<float32>(segmentID, "translationX");
		translationY = SharedMemoryManager::getSharedObject<float32>(segmentID, "translationY");
		translationZ = SharedMemoryManager::getSharedObject<float32>(segmentID, "translationZ");
	}

	if (rotationX && rotationY && rotationZ && translationX && translationY && translationZ)
	{
		Matrix4x4 mRotationX = Matrix4x4().setRotation(*rotationX, Matrix4x4::X_AXIS);
		Matrix4x4 mRotationY = Matrix4x4().setRotation(*rotationY, Matrix4x4::Y_AXIS);
		Matrix4x4 mRotationZ = Matrix4x4().setRotation(*rotationZ, Matrix4x4::Z_AXIS);
		Matrix4x4 mTranslation = Matrix4x4().setTranslation(Vector(*translationX, *translationY, *translationZ));
		kinectMatrix = mTranslation*mRotationZ*mRotationY*mRotationX;
		return true;
	}
	else
	{
		kinectMatrix.setIdentity();
		return false;
	}
}

void RenderSystemInterprocess::getTransformedKSkeletons(uint32& nSkeletons, KinectSkeleton* skeletons, int32 deviceIdx)
{
	if (deviceIdx == -1)
	{
		uint32 totalSkeletons = 0;
		std::vector< std::vector<KinectSkeleton> > skeletonCandidates(KINECT_SKELETON_COUNT);
		uint32 nKinects = KinectManager::getNumberOfDevices();
		for (uint32 i = 0; i < nKinects; i++)
		{
			std::string deviceID = KinectManager::getDeviceObject(i).getID();
			std::string segmentID = KinectManager::reformatDeviceID(deviceID);
			uint32* aux = SharedMemoryManager::getSharedObject<uint32>(segmentID, "nSkeletons");
			if (aux)
			{
				uint32 nSkeletonsAux = *aux;
				if (nSkeletonsAux > totalSkeletons) totalSkeletons = nSkeletonsAux;
				KinectSkeleton* skeletonsAux = SharedMemoryManager::getSharedObject<KinectSkeleton>(segmentID, "skeletons");
				Matrix4x4 kMatrix;
				getKMatrix(kMatrix, i);
				float32 psi, theta, phi;
				kMatrix.getEulerAngles(psi, theta, phi);
				Quaternion kQuaternion(phi, theta, psi);
				for (uint32 j = 0; j < nSkeletonsAux; j++)
				{
					uint32 s = basic_cast<uint32>(skeletonCandidates[j].size());
					skeletonCandidates[j].push_back(skeletonsAux[j]);
					for (uint32 k = 0; k < KINECT_SKELETON_JOINT_COUNT; k++)
					{
						KinectSkeleton::KinectJoint joint = basic_cast<KinectSkeleton::KinectJoint>(k);
						if (skeletonCandidates[j][s].getJointValidity(joint))
						{
							if (Config::kinect[deviceID].hierarchicalOri && joint != KinectSkeleton::K_HIP_CENTER)
							{
								skeletonCandidates[j][s].setJoint(
									joint,
									kMatrix*skeletonCandidates[j][s].getJointPosition(joint),
									skeletonCandidates[j][s].getJointOrientationQuaternion(joint));
							}
							else
							{
								skeletonCandidates[j][s].setJoint(
									joint,
									kMatrix*skeletonCandidates[j][s].getJointPosition(joint),
									kQuaternion*skeletonCandidates[j][s].getJointOrientationQuaternion(joint));
							}
						}
					}
				}
			}
		}

		nSkeletons = totalSkeletons;
		for (uint32 i = 0; i < nSkeletons; i++)
		{
			KinectSkeleton combinedSkeleton;
			combinedSkeleton.setPlayerIndex(i + 1);
			for (uint32 j = 0; j < KINECT_SKELETON_JOINT_COUNT; j++)
			{
				KinectSkeleton::KinectJoint joint = basic_cast<KinectSkeleton::KinectJoint>(j);
				float32 jointWeight = 0.0f;
				Point jointPosition;
				Quaternion jointOrientation;
				uint32 nJointCandidates = basic_cast<uint32>(skeletonCandidates[i].size());

				for (uint32 k = 0; k < nJointCandidates; k++)
				{
					if (skeletonCandidates[i][k].getJointValidity(joint))
					{
						float32 weight = skeletonCandidates[i][k].getJointWeight(joint)*(1.0f + skeletonCandidates[i][k].getConfidenceValue());
						jointWeight += weight;
						jointPosition += weight*skeletonCandidates[i][k].getJointPosition(joint);
						jointOrientation += weight*skeletonCandidates[i][k].getJointOrientationQuaternion(joint);
					}
				}
				if (jointWeight)
				{
					combinedSkeleton.setJoint(
						joint,
						jointPosition/jointWeight,
						jointOrientation/jointWeight);
				}
			}
			skeletons[i] = combinedSkeleton;
		}

	}
	else
	{
		std::string deviceID = KinectManager::getDeviceObject(deviceIdx).getID();
		std::string segmentID = KinectManager::reformatDeviceID(deviceID);
		uint32* aux = SharedMemoryManager::getSharedObject<uint32>(segmentID, "nSkeletons");
		if (aux)
		{
			nSkeletons = *aux;
			KinectSkeleton* originalSkeletons = SharedMemoryManager::getSharedObject<KinectSkeleton>(segmentID, "skeletons");
			Matrix4x4 kMatrix;
			getKMatrix(kMatrix, deviceIdx);
			float32 psi, theta, phi;
			kMatrix.getEulerAngles(psi, theta, phi);
			Quaternion kQuaternion(phi, theta, psi);
			for (uint32 i = 0; i < nSkeletons; i++)
			{
				skeletons[i] = originalSkeletons[i];
				for (uint32 j = 0; j < KINECT_SKELETON_JOINT_COUNT; j++)
				{
					KinectSkeleton::KinectJoint joint = basic_cast<KinectSkeleton::KinectJoint>(j);
					if (skeletons[i].getJointValidity(joint))
					{
						if (Config::kinect[deviceID].hierarchicalOri && joint != KinectSkeleton::K_HIP_CENTER)
						{
							skeletons[i].setJoint(
								joint,
								kMatrix*skeletons[i].getJointPosition(joint),
								skeletons[i].getJointOrientationQuaternion(joint));
						}
						else
						{
							skeletons[i].setJoint(
								joint,
								kMatrix*skeletons[i].getJointPosition(joint),
								kQuaternion*skeletons[i].getJointOrientationQuaternion(joint));
						}
					}
				}
			}
		}
		else nSkeletons = 0;
	}
}
