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


#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

/*
** Math definitions
*/
#define PI32 3.1415926535897932384626433832795f
#define PI64 3.1415926535897932384626433832795

#define DEG2RAD32(_X_) ((_X_)*PI32/180.0f)
#define DEG2RAD64(_X_) ((_X_)*PI64/180.0)
#define RAD2DEG32(_X_) ((_X_)*180.0f/PI32)
#define RAD2DEG64(_X_) ((_X_)*180.0/PI64)

/*
** Kinect definitions
*/
#define KINECT_SKELETON_COUNT		6	/* NUI_SKELETON_COUNT */
#define KINECT_SKELETON_JOINT_COUNT	20	/* NUI_SKELETON_POSITION_COUNT */
#define KINECT_MAX_TILT_ANGLE		27
#define KINECT_MIN_TILT_ANGLE		-27

/*
** Wiimote definitions
*/
#ifdef _WIIMOTE_SUPPORT_
#define WIIMOTE_COUNT 4
#endif

#endif
