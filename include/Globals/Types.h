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


#ifndef __TYPES_H__
#define __TYPES_H__

#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <Windows.h>


#define ANY2STR(_SRC_)	std::stringstream stream;	\
						stream << _SRC_;			\
						return stream.str()
#define ANY2WSTR(_SRC_)	std::wstringstream stream;	\
						stream << _SRC_;			\
						return stream.str()


namespace MultiKinect
{
	namespace Globals
	{
		/*
		** Basic types redefinition
		*/
		typedef int8_t		int8;
		typedef int16_t		int16;
		typedef int32_t		int32;
		typedef int64_t		int64;
		typedef uint8_t		uint8;
		typedef uint16_t	uint16;
		typedef uint32_t	uint32;
		typedef uint64_t	uint64;
		typedef float		float32;
		typedef double		float64;

		/*
		** Basic type casting functions
		*/
		template<typename T1, typename T2>	inline T1			basic_cast(T2 source);
		template<>							inline std::string	basic_cast<std::string, char>(char source);
		template<>							inline std::string	basic_cast<std::string, bool>(bool source);
		template<>							inline std::string	basic_cast<std::string, int8>(int8 source);
		template<>							inline std::string	basic_cast<std::string, int16>(int16 source);
		template<>							inline std::string	basic_cast<std::string, int32>(int32 source);
		template<>							inline std::string	basic_cast<std::string, int64>(int64 source);
		template<>							inline std::string	basic_cast<std::string, uint8>(uint8 source);
		template<>							inline std::string	basic_cast<std::string, uint16>(uint16 source);
		template<>							inline std::string	basic_cast<std::string, uint32>(uint32 source);
		template<>							inline std::string	basic_cast<std::string, uint64>(uint64 source);
		template<>							inline std::string	basic_cast<std::string, float32>(float32 source);
		template<>							inline std::string	basic_cast<std::string, float64>(float64 source);
		template<>							inline std::wstring	basic_cast<std::wstring, char>(char source);
		template<>							inline std::wstring	basic_cast<std::wstring, bool>(bool source);
		template<>							inline std::wstring	basic_cast<std::wstring, int8>(int8 source);
		template<>							inline std::wstring	basic_cast<std::wstring, int16>(int16 source);
		template<>							inline std::wstring	basic_cast<std::wstring, int32>(int32 source);
		template<>							inline std::wstring	basic_cast<std::wstring, int64>(int64 source);
		template<>							inline std::wstring	basic_cast<std::wstring, uint8>(uint8 source);
		template<>							inline std::wstring	basic_cast<std::wstring, uint16>(uint16 source);
		template<>							inline std::wstring	basic_cast<std::wstring, uint32>(uint32 source);
		template<>							inline std::wstring	basic_cast<std::wstring, uint64>(uint64 source);
		template<>							inline std::wstring	basic_cast<std::wstring, float32>(float32 source);
		template<>							inline std::wstring	basic_cast<std::wstring, float64>(float64 source);

		/*
		** String casting functions
		*/
		template<typename T>	inline T			string_cast(const std::string& source);
		template<>				inline std::wstring	string_cast<std::wstring>(const std::string& source);

		/*
		** Wide string casting functions
		*/
		template<typename T>	inline T			wstring_cast(const std::wstring& source);
		template<>				inline std::string	wstring_cast<std::string>(const std::wstring& source);

		/*
		** Enum casting functions
		*/
		template<typename T1, typename T2> inline T1 enum_cast(T2 source);

		/*
		** Useful string functions
		*/
		std::vector<std::string>	split(const std::string& source, const std::string& pattern);
		std::vector<std::wstring>	split(const std::wstring& source, const std::wstring& pattern);
		std::string					replace(const std::string& source, const std::string& what, const std::string& with);
		std::wstring				replace(const std::wstring& source, const std::wstring& what, const std::wstring& with);
	}
}


using namespace MultiKinect;
using namespace Globals;


/*
** Template functions implementation
*/
template<typename T1, typename T2> T1	Globals::basic_cast<T1, T2>(T2 source)						{ return static_cast<T1>(source); }
template<> std::string					Globals::basic_cast<std::string, char>(char source)			{ ANY2STR(source); }
template<> std::string					Globals::basic_cast<std::string, bool>(bool source)			{ ANY2STR(source); }
template<> std::string					Globals::basic_cast<std::string, int8>(int8 source)			{ ANY2STR(source); }
template<> std::string					Globals::basic_cast<std::string, int16>(int16 source)		{ ANY2STR(source); }
template<> std::string					Globals::basic_cast<std::string, int32>(int32 source)		{ ANY2STR(source); }
template<> std::string					Globals::basic_cast<std::string, int64>(int64 source)		{ ANY2STR(source); }
template<> std::string					Globals::basic_cast<std::string, uint8>(uint8 source)		{ ANY2STR(source); }
template<> std::string					Globals::basic_cast<std::string, uint16>(uint16 source)		{ ANY2STR(source); }
template<> std::string					Globals::basic_cast<std::string, uint32>(uint32 source)		{ ANY2STR(source); }
template<> std::string					Globals::basic_cast<std::string, uint64>(uint64 source)		{ ANY2STR(source); }
template<> std::string					Globals::basic_cast<std::string, float32>(float32 source)	{ ANY2STR(source); }
template<> std::string					Globals::basic_cast<std::string, float64>(float64 source)	{ ANY2STR(source); }
template<> std::wstring					Globals::basic_cast<std::wstring, char>(char source)		{ ANY2WSTR(source); }
template<> std::wstring					Globals::basic_cast<std::wstring, bool>(bool source)		{ ANY2WSTR(source); }
template<> std::wstring					Globals::basic_cast<std::wstring, int8>(int8 source)		{ ANY2WSTR(source); }
template<> std::wstring					Globals::basic_cast<std::wstring, int16>(int16 source)		{ ANY2WSTR(source); }
template<> std::wstring					Globals::basic_cast<std::wstring, int32>(int32 source)		{ ANY2WSTR(source); }
template<> std::wstring					Globals::basic_cast<std::wstring, int64>(int64 source)		{ ANY2WSTR(source); }
template<> std::wstring					Globals::basic_cast<std::wstring, uint8>(uint8 source)		{ ANY2WSTR(source); }
template<> std::wstring					Globals::basic_cast<std::wstring, uint16>(uint16 source)	{ ANY2WSTR(source); }
template<> std::wstring					Globals::basic_cast<std::wstring, uint32>(uint32 source)	{ ANY2WSTR(source); }
template<> std::wstring					Globals::basic_cast<std::wstring, uint64>(uint64 source)	{ ANY2WSTR(source); }
template<> std::wstring					Globals::basic_cast<std::wstring, float32>(float32 source)	{ ANY2WSTR(source); }
template<> std::wstring					Globals::basic_cast<std::wstring, float64>(float64 source)	{ ANY2WSTR(source); }

template<typename T>
T Globals::string_cast(const std::string& source)
{
	std::stringstream stream;
	stream << source;
	T result;
	stream >> result;
	return result;
}

template<>
std::wstring Globals::string_cast<std::wstring>(const std::string& source)
{
	if (source.length())
	{
		uint32 wLength = ::MultiByteToWideChar(CP_ACP, 0, &source[0], basic_cast<int32>(source.length()), 0, 0);
		if (wLength)
		{
			std::wstring result(wLength, 0);
			::MultiByteToWideChar(CP_ACP, 0, &source[0], basic_cast<int32>(source.length()), &result[0], wLength);
			return result;
		}
	}

	return std::wstring();
}

template<typename T>
T Globals::wstring_cast(const std::wstring& source)
{
	std::wstringstream stream;
	stream << source;
	T result;
	stream >> result;
	return result;
}

template<>
std::string Globals::wstring_cast<std::string>(const std::wstring& source)
{
	if (source.length())
	{
		uint32 length = ::WideCharToMultiByte(CP_ACP, 0, &source[0], basic_cast<int32>(source.length()), 0, 0, 0, 0);
		if (length)
		{
			std::string result(length, 0);
			::WideCharToMultiByte(CP_ACP, 0, &source[0], basic_cast<int32>(source.length()), &result[0], length, 0, 0);
			return result;
		}
	}

	return std::string();
}

template<typename T1, typename T2>
T1 Globals::enum_cast(T2 source)
{
	return basic_cast<T1>(static_cast<int32>(source));
}

#endif
