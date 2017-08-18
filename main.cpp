#include <Objbase.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <wincon.h>


#include "window.h"


#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Rpcrt4.lib")


const std::wstring ToString(const GUID &guid)
{
	RPC_WSTR guidStr = nullptr;
	UuidToStringW(&guid, &guidStr);
	const auto result = std::wstring(reinterpret_cast<const wchar_t*>(guidStr));
	RpcStringFreeW(&guidStr);

	return result;
}

std::wstring GenerateGuardGuid()
{
	GUID guid = {};
	CoCreateGuid(&guid);
	auto strGuid = ToString(guid);

	std::transform(strGuid.begin(), strGuid.end(), strGuid.begin(), ::toupper);
	std::replace(strGuid.begin(), strGuid.end(), L'-', L'_');

	return std::wstring(L"G") + strGuid;
}

void CopyToClipboard(const std::wstring &data, HWND hwnd)
{
	if (!OpenClipboard(hwnd))
		throw std::runtime_error("can't open clipboard");
	if (!EmptyClipboard())
		throw std::runtime_error("can't clear clipboard");

	const auto dataBytes	= data.length() * sizeof(data[0]);
	const auto memSize		= dataBytes + 2 * sizeof(data[0]);

	auto globMem = GlobalAlloc(GMEM_MOVEABLE, memSize);
	if (!globMem)
		return;

	auto lockedMem = GlobalLock(globMem);
	{
		memcpy(lockedMem, data.c_str(), dataBytes);
		memset(static_cast<uint8_t*>(lockedMem) + dataBytes, 0, memSize - dataBytes);
	}
	GlobalUnlock(globMem);

	SetClipboardData(CF_UNICODETEXT, globMem);

	CloseClipboard();
}


int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR cmd, int iShow)
try
{
	const Window win(hinst);

	const auto guardGuid = GenerateGuardGuid();
	std::wstringstream ss;

	if (cmd && (strstr(cmd, "-guardonly") > 0))
		ss << guardGuid;
	else
		ss	<< L"#ifndef " << guardGuid << L"\r\n"
			<< L"#define " << guardGuid << L"\r\n"
			<< L"\r\n"
			<< L"\r\n"
			<< L"\r\n"
			<< L"\r\n"
			<< L"\r\n"
			<< L"#endif//" << guardGuid << L"\r\n";

	CopyToClipboard(ss.str(), win.GetHwnd());

	return 0;
}
catch (const std::exception &ex)
{
	std::stringstream ss;
	ss << ">!> EXCEPTION: " << ex.what() << "\n";
	OutputDebugStringA(ss.str().c_str());

	return -1;
}