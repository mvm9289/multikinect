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


#ifndef __KINECTSKELETON_H__
#define __KINECTSKELETON_H__

#include "Globals/Include.h"
#include "Geom/Point.h"
#include "Geom/Quaternion.h"
#include <vector>
#include <Windows.h>
#include <NuiApi.h>


namespace MultiKinect
{
	namespace Kinect
	{
		class KinectSkeleton
		{
		public:
			enum KinectJoint
			{
				K_NONE = -1,
				K_HEAD = 0,
				K_SHOULDER_LEFT = 1,
				K_SHOULDER_CENTER = 2,
				K_SHOULDER_RIGHT = 3,
				K_ELBOW_LEFT = 4,
				K_ELBOW_RIGHT = 5,
				K_WRIST_LEFT = 6,
				K_WRIST_RIGHT = 7,
				K_HAND_LEFT = 8,
				K_HAND_RIGHT = 9,
				K_SPINE = 10,
				K_HIP_LEFT = 11,
				K_HIP_CENTER = 12,
				K_HIP_RIGHT = 13,
				K_KNEE_LEFT = 14,
				K_KNEE_RIGHT = 15,
				K_ANKLE_LEFT = 16,
				K_ANKLE_RIGHT = 17,
				K_FOOT_LEFT = 18,
				K_FOOT_RIGHT = 19
			};

			enum KinectSegment
			{
				 K_BODY = 0,
				 K_ARM_LEFT = 1,
				 K_ARM_RIGHT = 2,
				 K_LEG_LEFT = 3,
				 K_LEG_RIGHT = 4
			};

		private:
			static _NUI_IMAGE_RESOLUTION resolution_;

			uint32 playerIndex_;
			Point joints_[KINECT_SKELETON_JOINT_COUNT];
			Quaternion jointOrientations_[KINECT_SKELETON_JOINT_COUNT];
			bool validJoints_[KINECT_SKELETON_JOINT_COUNT];
			bool lastValidJoints_[KINECT_SKELETON_JOINT_COUNT];
			float32 confidenceValue_;
			float32 lastJointsTimestamps_[KINECT_SKELETON_JOINT_COUNT];

		public:
			KinectSkeleton();
			virtual ~KinectSkeleton();

			static void setResolution(uint32 width, uint32 height);
			static Point transformToDepthPoint(const Point& p);

			uint32 getPlayerIndex() const;
			Color getPlayerColor() const;
			Point getJointPosition(KinectJoint joint) const;
			Matrix3x3 getJointOrientationMatrix(KinectJoint joint) const;
			Quaternion getJointOrientationQuaternion(KinectJoint joint) const;
			bool getJointValidity(KinectJoint joint) const;
			float32 getJointWeight(KinectJoint joint) const;
			std::vector<Point> getJointsPositions() const;
			std::vector<Matrix3x3> getJointsOrientationsMatrices() const;
			std::vector<Quaternion> getJointsOrientationsQuaternions() const;
			std::vector<bool> getJointsValidity() const;
			std::vector<float32> getJointsWeights() const;
			std::vector<Point> getSegment(KinectSegment segment) const;
			std::vector< std::vector<Point> > getBonesSegments() const;
			void setPlayerIndex(uint32 playerIndex);
			void setJoint(KinectJoint joint, const Point& point, const Quaternion& orientation);
			float32 getConfidenceValue();
			void clear();
		};
	}
}

#endif
