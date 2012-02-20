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


#include "Tools/Timer.h"

#include <wx/stopwatch.h>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace MultiKinect;
using namespace Tools;


std::map<std::string, uint64> Timer::events;

void Timer::startCount(std::string id)
{
	events[id] = wxGetLocalTimeMillis().GetValue();
}

uint64 Timer::resetCount(std::string id)
{
	uint64 now = wxGetLocalTimeMillis().GetValue();
	uint64 ret = now - events[id];
	events[id] = now;
	return ret;
}

uint64 Timer::endCount(std::string id)
{
	uint64 ret = wxGetLocalTimeMillis().GetValue() - events[id];
	events.erase(id);
	return ret;
}

uint64 Timer::getCountTime(std::string id)
{
	return wxGetLocalTimeMillis().GetValue() - events[id];
}

std::string Timer::getCountStrHMS(std::string id)
{
	uint64 start = events[id];
	uint32 hours = basic_cast<uint32>((wxGetLocalTimeMillis().GetValue() - start)/(1000*60*60));
	uint32 minutes = basic_cast<uint32>(((wxGetLocalTimeMillis().GetValue() - start)%(1000*60*60))/(1000*60));
	uint32 seconds = basic_cast<uint32>(((wxGetLocalTimeMillis().GetValue() - start)%(1000*60))/1000);

	std::stringstream ss;
	ss  << "["
		<< std::setw(2)
		<< std::setfill('0')
		<< std::setiosflags(std::stringstream::right)
		<< hours
		<< ":"
		<< std::setw(2)
		<< std::setfill('0')
		<< std::setiosflags(std::stringstream::right)
		<< minutes
		<< ":"
		<< std::setw(2)
		<< std::setfill('0')
		<< std::setiosflags(std::stringstream::right)
		<< seconds
		<< "]";

	return ss.str();
}

std::string Timer::getSysDateDDMMYY()
{
	char buffer[9];
	time_t rawtime;
	struct tm*  timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	strftime(buffer, 9, "%x", timeinfo);

	return std::string(buffer);
}

std::string Timer::getSysTimeHHMMSS()
{
	char buffer[9];
	time_t rawtime;
	struct tm*  timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	strftime(buffer, 9, "%X", timeinfo);

	return std::string(buffer);
}

std::string Timer::getSysDateTimeID()
{
	std::string dateStr = getSysDateDDMMYY();
	dateStr.erase(5, 1);
	dateStr.erase(2, 1);

	std::string timeStr = getSysTimeHHMMSS();
	timeStr.erase(5, 1);
	timeStr.erase(2, 1);

	return (dateStr + "_" + timeStr);
}

void Timer::wait(uint64 milliseconds)
{
	startCount("__WAIT__");
	while (getCountTime("__WAIT__") < milliseconds);
	endCount("__WAIT__");
}
