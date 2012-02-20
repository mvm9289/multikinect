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


#ifndef __RENDERSYSTEMLOCAL_H__
#define __RENDERSYSTEMLOCAL_H__

#include "Globals/Include.h"
#include "Render/RenderSystem.h"


namespace MultiKinect
{
	namespace Render
	{
		class RenderSystemLocal : public RenderSystem
		{
		private:
			KinectDevice* device_;

		public:
			RenderSystemLocal(KinectDevice* device);
			virtual ~RenderSystemLocal();

			KinectDevice* getDevice();

			virtual uint8* getKColorFrame(int32 deviceIdx = -1);
			virtual uint8* getKDepthFrame(int32 deviceIdx = -1);
			virtual void getKSkeletons(uint32& nSkeletons, KinectSkeleton*& skeletons, int32 deviceIdx = -1);
			virtual bool getKMatrix(Matrix4x4& kinectMatrix, int32 deviceIdx = -1);
			virtual void getTransformedKSkeletons(uint32& nSkeletons, KinectSkeleton* skeletons, int32 deviceIdx = -1);
		};
	}
}

#endif
