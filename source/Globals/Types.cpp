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


#include "Globals/Types.h"


std::vector<std::string> Globals::split(const std::string& source, const std::string& pattern)
{
	std::vector<std::string> result;
	std::string sourceString	=	source;
	int32 patternPosition		=	basic_cast<int32>(sourceString.find(pattern));
	while (patternPosition != std::string::npos)
	{
		if (patternPosition != 0) result.push_back(sourceString.substr(0, patternPosition));

		sourceString	= sourceString.substr(patternPosition + pattern.length());
		patternPosition	= basic_cast<int32>(sourceString.find(pattern));
	}
	result.push_back(sourceString);

	return result;
}

std::vector<std::wstring> Globals::split(const std::wstring& source, const std::wstring& pattern)
{
	std::vector<std::wstring> result;
	std::wstring sourceString	=	source;
	int32 patternPosition		=	basic_cast<int32>(sourceString.find(pattern));
	while (patternPosition != std::wstring::npos)
	{
		if (patternPosition != 0) result.push_back(sourceString.substr(0, patternPosition));

		sourceString	= sourceString.substr(patternPosition + pattern.length());
		patternPosition	= basic_cast<int32>(sourceString.find(pattern));
	}
	result.push_back(sourceString);

	return result;
}

std::string Globals::replace(const std::string& source, const std::string& what, const std::string& with)
{
	std::string result	= source;
	int32 whatPosition	= basic_cast<int32>(result.find(what));
	while (whatPosition != std::string::npos)
	{
		result.replace(whatPosition, what.size(), with);
		whatPosition = basic_cast<int32>(result.find(what));
	}

	return result;
}

std::wstring Globals::replace(const std::wstring& source, const std::wstring& what, const std::wstring& with)
{
	std::wstring result	= source;
	int32 whatPosition	= basic_cast<int32>(result.find(what));
	while (whatPosition != std::string::npos)
	{
		result.replace(whatPosition, what.size(), with);
		whatPosition = basic_cast<int32>(result.find(what));
	}

	return result;
}
