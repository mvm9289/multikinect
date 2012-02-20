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


#ifndef __RENDERSYSTEM_H__
#define __RENDERSYSTEM_H__

#include "Globals/Include.h"


namespace MultiKinect
{
	namespace Render
	{
		class RenderSystem
		{
		public:
			enum RenderSystemMode
			{
				RS_LOCAL_DEVICE = 0,
				RS_INTERPROCESS,
				RS_REMOTE_DEVICE
			};

		private:
			static RenderSystem* instance_;

		public:
			static void initialize(RenderSystemMode mode, KinectDevice* device = 0);
			static void destroy();

			static bool isInitialized();
			static bool hasDevice();
			static KinectDevice* getDevice();
			static uint8* getKinectColorFrame(int32 deviceIdx = -1);
			static uint8* getKinectDepthFrame(int32 deviceIdx = -1);
			static KinectSkeleton* getKinectSkeleton(uint32 i, int32 deviceIdx = -1);
			static void getKinectSkeletons(uint32& nSkeletons, KinectSkeleton*& skeletons, int32 deviceIdx = -1);
			static bool getKinectMatrix(Matrix4x4& kinectMatrix, int32 deviceIdx = -1);
			static bool getTransformedKinectSkeleton(KinectSkeleton& skeleton, uint32 i, int32 deviceIdx = -1);
			static void getTransformedKinectSkeletons(uint32& nSkeletons, KinectSkeleton* skeletons, int32 deviceIdx = -1);
			static float32 getKinectFPS();

			RenderSystem();
			virtual ~RenderSystem();

			virtual uint8* getKColorFrame(int32 deviceIdx = -1) = 0;
			virtual uint8* getKDepthFrame(int32 deviceIdx = -1) = 0;
			virtual void getKSkeletons(uint32& nSkeletons, KinectSkeleton*& skeletons, int32 deviceIdx = -1) = 0;
			virtual bool getKMatrix(Matrix4x4& kinectMatrix, int32 deviceIdx = -1);
			virtual void getTransformedKSkeletons(uint32& nSkeletons, KinectSkeleton* skeletons, int32 deviceIdx = -1);
		};
	}
}

#endif
