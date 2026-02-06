#include "FileSystem.h"

static void NormalizePathSeparators(std::wstring& path)
{
	for (auto& charachter : path)
	{
		if (charachter == L'\\')
			charachter = L'/';
	}
}

std::wstring FileSystem::GetExecutablePath()
{
	wchar_t buffer[MAX_PATH];

	DWORD length =
		GetModuleFileNameW(nullptr, buffer, MAX_PATH);

	if (length == 0)
		return L"";

	std::wstring executablePath(buffer, length);

	NormalizePathSeparators(executablePath);
	return executablePath;
}

std::wstring FileSystem::GetExecutableDirectoryPath()
{
	std::wstring executablePath = GetExecutablePath();

	if (executablePath.empty())
		return L"";

	size_t pos = executablePath.find_last_of(L"\\/");

	if (pos == std::wstring::npos)
		return L"";

	std::wstring directoryPath = executablePath.substr(0, pos + 1);

	NormalizePathSeparators(directoryPath);
	return directoryPath;
}