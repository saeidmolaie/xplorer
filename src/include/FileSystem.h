#pragma once

#include <windows.h>
#include <string>

class FileSystem
{

public:
	FileSystem() = delete;
	FileSystem(const FileSystem&) = delete;
	FileSystem& operator=(const FileSystem&) = delete;
	FileSystem(FileSystem&&) = delete;
	FileSystem& operator=(FileSystem&&) = delete;
	~FileSystem() = delete;

public:
	static std::wstring GetExecutablePath();
	static std::wstring GetExecutableDirectoryPath();

};