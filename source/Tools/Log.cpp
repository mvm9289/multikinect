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


#include "Tools/Log.h"

#include "Globals/Config.h"


bool			Log::initialized_ = false;
std::string		Log::logFilename_ = "";
std::fstream	Log::logFile_;


void Log::initialize(const std::string& logID)
{
	if (!initialized_)
	{
		if (logID != "")	logFilename_ = Config::system.logPath + "/MultiKinect_Log_" + logID + ".txt";
		else				logFilename_ = Config::system.logPath + "/MultiKinect_Log.txt";

		logFile_.open(logFilename_, std::fstream::out);

		std::string text = Timer::getCountStrHMS("executionStart") + " -> Application started.\n";
		logFile_.write(text.c_str(), text.length());

		logFile_.close();
		logFile_.clear();

		initialized_ = true;
	}
	else Log::write("[Log] initialize()", "ERROR: Log already initialized.");
}

void Log::destroy()
{
	if (initialized_)
	{
		logFile_.open(logFilename_, std::fstream::out|std::fstream::app);

		std::string text = Timer::getCountStrHMS("executionStart") + " -> Application exited.\n";
		logFile_.write(text.c_str(), text.length());

		logFile_.close();
		logFile_.clear();

		initialized_ = false;
	}
	else std::cout << "[Log] destroy(): ERROR: Log not initialized." << std::endl;
}

bool Log::isInitialized()
{
	return initialized_;
}

std::string Log::getLogFilename()
{
	if (initialized_) return logFilename_;
	else
	{
		std::cout << "[Log] getLogFilename(): ERROR: Log not initialized." << std::endl;
		return "";
	}
}

void Log::write(std::string message)
{
	if (initialized_)
	{
		logFile_.open(logFilename_, std::fstream::out|std::fstream::app);

		std::stringstream stream;
		stream << Timer::getCountStrHMS("executionStart") << " -> " << message << std::endl;
		logFile_.write(stream.str().c_str(), stream.str().length());

		logFile_.close();
		logFile_.clear();
	}
	else std::cout << "[Log] write(): ERROR: Log not initialized." << std::endl;
}

void Log::wrap(uint32 lines)
{
	if (initialized_)
	{
		logFile_.open(logFilename_, std::fstream::out|std::fstream::app);

		for (uint32 i = 0; i < lines; i++) logFile_.write("\n", 1);

		logFile_.close();
		logFile_.clear();
	}
	else std::cout << "[Log] wrap(): ERROR: Log not initialized." << std::endl;
}

std::string Log::getLog()
{
	if (initialized_)
	{
		logFile_.open(logFilename_, std::fstream::in);

		logFile_.seekg(0, std::fstream::end);
		uint32 length = basic_cast<uint32>(logFile_.tellg());
		logFile_.seekg(0, std::fstream::beg);

		char* buffer = new char[length];
		logFile_.read(buffer, length);
		buffer[length - 1] = 0;
		std::string result(buffer);
		delete[] buffer;

		logFile_.close();
		logFile_.clear();

		return result;
	}
	else
	{
		std::cout << "[Log] readLog(): ERROR: Log not initialized." << std::endl;
		return "";
	}
}
