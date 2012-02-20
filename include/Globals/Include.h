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

#include "Globals/Definitions.h"
#include "Globals/Types.h"
#include "Globals/Objects.h"
#include "Globals/Vars.h"

#ifndef _NDEBUG_
#include <vld.h>
#endif

using namespace MultiKinect;
using namespace Globals;
using namespace Files;
using namespace Geom;
using namespace GUI;
using namespace Kinect;
using namespace Interprocess;
using namespace Render;
using namespace Tools;
using namespace VRPN;
