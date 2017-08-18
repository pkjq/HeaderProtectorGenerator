#ifndef GUARD_C2B24404_2154_4DB5_A16E_03F68D38F611
#define GUARD_C2B24404_2154_4DB5_A16E_03F68D38F611


#include <windows.h>
#include <thread>
#include <atomic>


class Window
{
public:
	explicit Window(HINSTANCE hinst);
	~Window();

public:
	HWND GetHwnd() const;

private:
	HWND Create(HINSTANCE hinst);
	void MainLoop();

private:
	std::atomic<HWND> hwnd = { nullptr };
	std::thread wndThread;
};


#endif//GUARD_C2B24404_2154_4DB5_A16E_03F68D38F611
