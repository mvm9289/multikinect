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


#include "Kinect/KinectSkeleton.h"

#include "Geom/Point.h"
#include "Geom/Color.h"
#include "Geom/Matrix3x3.h"
#include "Globals/Config.h"
#include "Globals/Definitions.h"
#include "Tools/Timer.h"


_NUI_IMAGE_RESOLUTION KinectSkeleton::resolution_ = NUI_IMAGE_RESOLUTION_INVALID;


KinectSkeleton::KinectSkeleton()
{
	playerIndex_ = 0;
	std::memset(joints_, 0, sizeof(Point)*KINECT_SKELETON_JOINT_COUNT);
	std::memset(jointOrientations_, 0, sizeof(Quaternion)*KINECT_SKELETON_JOINT_COUNT);
	std::memset(validJoints_, 0, sizeof(bool)*KINECT_SKELETON_JOINT_COUNT);
	std::memset(lastValidJoints_, 0, sizeof(bool)*KINECT_SKELETON_JOINT_COUNT);
	confidenceValue_ = -1.0f;
	std::memset(lastJointsTimestamps_, 0, sizeof(float32)*KINECT_SKELETON_JOINT_COUNT);
}

KinectSkeleton::~KinectSkeleton()
{
}

void KinectSkeleton::setResolution(uint32 width, uint32 height)
{
	resolution_ = NUI_IMAGE_RESOLUTION_INVALID;
	switch (width)
	{
	case 80:
		if (height == 60) resolution_ = NUI_IMAGE_RESOLUTION_80x60;
		break;
	case 320:
		if (height == 240) resolution_ = NUI_IMAGE_RESOLUTION_320x240;
		break;
	case 640:
		if (height == 480) resolution_ = NUI_IMAGE_RESOLUTION_640x480;
		break;
	default:
		break;
	}
}

Point KinectSkeleton::transformToDepthPoint(const Point& p)
{
	Vector4 point;
	point.x = p.x;
	point.y = p.y;
	point.z = p.z;
	point.w = 1.0f;

	Point result(0.0f, 0.0f, 0.0f);
	NuiTransformSkeletonToDepthImage(point, &result.x, &result.y, resolution_);

	return result;
}

uint32 KinectSkeleton::getPlayerIndex() const
{
	return playerIndex_;
}

Color KinectSkeleton::getPlayerColor() const
{
	float32 red = 0.0f;
	float32 green = 0.0f;
	float32 blue = 0.0f;
	uint32 playerIndex = playerIndex_;
	if (!playerIndex) playerIndex = 0x0007;
	if (playerIndex&0x0001) red = 1.0f;
	if (playerIndex&0x0002) green = 1.0f;
	if (playerIndex&0x0004) blue = 1.0f;
	return Color(red, green, blue);
}

Point KinectSkeleton::getJointPosition(KinectJoint joint) const
{
	return joints_[joint];
}

Matrix3x3 KinectSkeleton::getJointOrientationMatrix(KinectJoint joint) const
{
	return jointOrientations_[joint].rotationMat3();
}

Quaternion KinectSkeleton::getJointOrientationQuaternion(KinectJoint joint) const
{
	return jointOrientations_[joint];
}

bool KinectSkeleton::getJointValidity(KinectJoint joint) const
{
	return validJoints_[joint];
}

float32 KinectSkeleton::getJointWeight(KinectJoint joint) const
{
	if (validJoints_[joint])
		return (basic_cast<float32>(basic_cast<float64>(Timer::getCountTime("executionStart"))*0.001) - lastJointsTimestamps_[joint]);
	else return 0.0f;
}

std::vector<Point> KinectSkeleton::getJointsPositions() const
{
	std::vector<Point> result(KINECT_SKELETON_JOINT_COUNT);
	for (uint32 i = 0; i < KINECT_SKELETON_JOINT_COUNT; i++)
		result[i] = joints_[i];

	return result;
}

std::vector<Matrix3x3> KinectSkeleton::getJointsOrientationsMatrices() const
{
	std::vector<Matrix3x3> result(KINECT_SKELETON_JOINT_COUNT);
	for (uint32 i = 0; i < KINECT_SKELETON_JOINT_COUNT; i++)
		result[i] = jointOrientations_[i].rotationMat3();

	return result;
}

std::vector<Quaternion> KinectSkeleton::getJointsOrientationsQuaternions() const
{
	std::vector<Quaternion> result(KINECT_SKELETON_JOINT_COUNT);
	for (uint32 i = 0; i < KINECT_SKELETON_JOINT_COUNT; i++)
		result[i] = jointOrientations_[i];

	return result;
}

std::vector<bool> KinectSkeleton::getJointsValidity() const
{
	std::vector<bool> result(KINECT_SKELETON_JOINT_COUNT);
	for (uint32 i = 0; i < KINECT_SKELETON_JOINT_COUNT; i++)
		result[i] = validJoints_[i];

	return result;
}

std::vector<float32> KinectSkeleton::getJointsWeights() const
{
	float32 currentTimestamp = basic_cast<float32>(basic_cast<float64>(Timer::getCountTime("executionStart"))*0.001);
	std::vector<float32> result(KINECT_SKELETON_JOINT_COUNT);
	for (uint32 i = 0; i < KINECT_SKELETON_JOINT_COUNT; i++)
	{
		if (validJoints_[i]) result[i] = currentTimestamp - lastJointsTimestamps_[i];
		else result[i] = 0.0f;
	}

	return result;
}

std::vector<Point> KinectSkeleton::getSegment(KinectSegment segment) const
{
	std::vector<Point> result;

	switch (segment)
	{
	case K_BODY:
		if (validJoints_[K_HEAD]) result.push_back(joints_[K_HEAD]);
		if (validJoints_[K_SHOULDER_CENTER]) result.push_back(joints_[K_SHOULDER_CENTER]);
		if (validJoints_[K_SPINE]) result.push_back(joints_[K_SPINE]);
		if (validJoints_[K_HIP_CENTER]) result.push_back(joints_[K_HIP_CENTER]);
		break;
	case K_ARM_LEFT:
		if (validJoints_[K_SHOULDER_CENTER]) result.push_back(joints_[K_SHOULDER_CENTER]);
		if (validJoints_[K_SHOULDER_LEFT]) result.push_back(joints_[K_SHOULDER_LEFT]);
		if (validJoints_[K_ELBOW_LEFT]) result.push_back(joints_[K_ELBOW_LEFT]);
		if (validJoints_[K_WRIST_LEFT]) result.push_back(joints_[K_WRIST_LEFT]);
		if (validJoints_[K_HAND_LEFT]) result.push_back(joints_[K_HAND_LEFT]);
		break;
	case K_ARM_RIGHT:
		if (validJoints_[K_SHOULDER_CENTER]) result.push_back(joints_[K_SHOULDER_CENTER]);
		if (validJoints_[K_SHOULDER_RIGHT]) result.push_back(joints_[K_SHOULDER_RIGHT]);
		if (validJoints_[K_ELBOW_RIGHT]) result.push_back(joints_[K_ELBOW_RIGHT]);
		if (validJoints_[K_WRIST_RIGHT]) result.push_back(joints_[K_WRIST_RIGHT]);
		if (validJoints_[K_HAND_RIGHT]) result.push_back(joints_[K_HAND_RIGHT]);
		break;
	case K_LEG_LEFT:
		if (validJoints_[K_HIP_CENTER]) result.push_back(joints_[K_HIP_CENTER]);
		if (validJoints_[K_HIP_LEFT]) result.push_back(joints_[K_HIP_LEFT]);
		if (validJoints_[K_KNEE_LEFT]) result.push_back(joints_[K_KNEE_LEFT]);
		if (validJoints_[K_ANKLE_LEFT]) result.push_back(joints_[K_ANKLE_LEFT]);
		if (validJoints_[K_FOOT_LEFT]) result.push_back(joints_[K_FOOT_LEFT]);
		break;
	case K_LEG_RIGHT:
		if (validJoints_[K_HIP_CENTER]) result.push_back(joints_[K_HIP_CENTER]);
		if (validJoints_[K_HIP_RIGHT]) result.push_back(joints_[K_HIP_RIGHT]);
		if (validJoints_[K_KNEE_RIGHT]) result.push_back(joints_[K_KNEE_RIGHT]);
		if (validJoints_[K_ANKLE_RIGHT]) result.push_back(joints_[K_ANKLE_RIGHT]);
		if (validJoints_[K_FOOT_RIGHT]) result.push_back(joints_[K_FOOT_RIGHT]);
		break;
	default:
		break;
	}

	if (result.size() <= 1) result.clear();
		
	return result;
}

std::vector< std::vector<Point> > KinectSkeleton::getBonesSegments() const
{
	std::vector< std::vector<Point> > result;

	result.push_back(getSegment(K_BODY));
	result.push_back(getSegment(K_ARM_LEFT));
	result.push_back(getSegment(K_ARM_RIGHT));
	result.push_back(getSegment(K_LEG_LEFT));
	result.push_back(getSegment(K_LEG_RIGHT));

	return result;
}

void KinectSkeleton::setPlayerIndex(uint32 playerIndex)
{
	playerIndex_ = playerIndex;
}

void KinectSkeleton::setJoint(KinectJoint joint, const Point& point, const Quaternion& orientation)
{
	joints_[joint] = point;
	jointOrientations_[joint] = orientation;
	validJoints_[joint] = true;
	if (!lastValidJoints_[joint])
	{
		lastJointsTimestamps_[joint] = basic_cast<float32>(basic_cast<float64>(Timer::getCountTime("executionStart"))*0.001);
		lastValidJoints_[joint] = true;
	}
}

float32 KinectSkeleton::getConfidenceValue()
{
	if (confidenceValue_ == -1.0f)
	{
		confidenceValue_ = 0.0f;
		std::vector<float32> jointWeights = getJointsWeights();
		for (uint32 i = 0; i < KINECT_SKELETON_JOINT_COUNT; i++)
			confidenceValue_ += jointWeights[i];
		confidenceValue_ /= KINECT_SKELETON_JOINT_COUNT;
	}
	return confidenceValue_;
}

void KinectSkeleton::clear()
{
	std::memcpy(lastValidJoints_, validJoints_, sizeof(bool)*KINECT_SKELETON_JOINT_COUNT);
	std::memset(validJoints_, 0, sizeof(bool)*KINECT_SKELETON_JOINT_COUNT);
	confidenceValue_ = -1.0f;
}
