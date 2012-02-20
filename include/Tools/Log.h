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


#ifndef __LOG_H__
#define __LOG_H__


#include "Globals/Include.h"
#include "Tools/Timer.h"
#include <iostream>
#include <fstream>


namespace MultiKinect
{
	namespace Tools
	{
		class Log
		{
		private:
			static bool			initialized_;
			static std::string	logFilename_;
			static std::fstream	logFile_;

		public:
			static void initialize(const std::string& logID = "");
			static void destroy();

			static bool			isInitialized();
			static std::string	getLogFilename();
			static void			write(std::string message);
			static void			wrap(uint32 lines = 1);
			static std::string	getLog();

			template<typename T>
			static void write(std::string message, const T& var)
			{
				if (initialized_)
				{
					logFile_.open(logFilename_, std::fstream::out|std::fstream::app);

					std::stringstream stream;
					stream << Timer::getCountStrHMS("executionStart") << " -> " << message << ": " << var << std::endl;
					logFile_.write(stream.str().c_str(), stream.str().length());

					logFile_.close();
					logFile_.clear();
				}
				else std::cout << "[Log] write(): ERROR: Log not initialized." << std::endl;
			}
		};
	}
}

#endif
