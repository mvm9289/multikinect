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


#include "Files/Filesystem.h"

#include "Globals/Config.h"
#include "Tools/Log.h"
#include <Windows.h>
#include <Shlobj.h>
#include <direct.h>


bool Filesystem::initialized_ = false;


void Filesystem::initialize()
{
	if (!initialized_) initialized_ = true;
	else Log::write("[Filesystem] initialize()", "ERROR: Filesystem already initialized.");
}

void Filesystem::destroy()
{
	if (initialized_)	initialized_ = false;
	else Log::write("[Filesystem] destroy()", "ERROR: Filesystem not initialized.");
}

bool Filesystem::isInitialized()
{
	return initialized_;
}

bool Filesystem::fileExists(const std::string& path)
{
	uint32 fileAttributes = GetFileAttributesA(path.c_str());
	if (fileAttributes == INVALID_FILE_ATTRIBUTES || (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)) return false;
	return true;
}

bool Filesystem::directoryExists(const std::string& path)
{
	uint32 fileAttributes = GetFileAttributesA(path.c_str());
	if (fileAttributes == INVALID_FILE_ATTRIBUTES || !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY)) return false;
	return true;
}

void Filesystem::createDirectory(const std::string& path)
{
	if (path != "" && !directoryExists(path) && !fileExists(path))
	{
		std::string asdf = replace(path, "\\", "/");
		std::vector<std::string> directoryTree = split(replace(path, "\\", "/"), "/");

		std::string partialPath = directoryTree[0];
		if (!directoryExists(partialPath)) _mkdir(partialPath.c_str());

		for (uint32 i = 1; i < directoryTree.size(); i++)
		{
			partialPath += ("/" + directoryTree[i]);
			if (!directoryExists(partialPath)) _mkdir(partialPath.c_str());
		}
	}
}

std::string Filesystem::getLocalDataPath()
{
	TCHAR localAppDataPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(0, CSIDL_LOCAL_APPDATA, 0, 0, localAppDataPath)))
	{
		std::string localDataPath = wstring_cast<std::string>(std::wstring(localAppDataPath));
		if (localDataPath != "") return localDataPath;
	}

	return Config::DEFAULT_APPDATA_PATH;
}

std::string MultiKinect::Files::Filesystem::getExecutablePath()
{
	HMODULE module = GetModuleHandleW(NULL);
	TCHAR execFilename[MAX_PATH];
	if (SUCCEEDED(GetModuleFileNameW(module, execFilename, MAX_PATH)))
	{
		std::string execPath = wstring_cast<std::string>(std::wstring(execFilename));
		size_t pos = execPath.find("MultiKinect.exe");
		if (pos != std::string::npos && pos > 0)
			return execPath.substr(0, pos - 1);
	}

	return "./";
}
