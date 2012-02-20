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


#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "Globals/Include.h"


namespace MultiKinect
{
	namespace Files
	{
		class Filesystem
		{
		private:
			static bool initialized_;

		public:
			static void initialize();
			static void destroy();

			static bool			isInitialized();
			static bool			fileExists(const std::string& path);
			static bool			directoryExists(const std::string& path);
			static void			createDirectory(const std::string& path);
			static std::string	getLocalDataPath();
			static std::string	getExecutablePath();
		};
	}
}

#endif
