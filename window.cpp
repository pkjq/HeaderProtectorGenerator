#include "window.h"
#include <future>


Window::Window(HINSTANCE hinst)
{
	std::atomic_bool winCreated = { false };

	std::packaged_task<void()> task([&]()
	{
		const auto h = Create(hinst);

		hwnd.store(h, std::memory_order_relaxed);
		winCreated.store(true, std::memory_order_release);

		if (h)
			MainLoop();
	});

	auto result = task.get_future();
	wndThread = std::move(std::thread(std::move(task)));

	while (!winCreated.load(std::memory_order_acquire))
		std::this_thread::yield();
	if (!hwnd.load(std::memory_order_relaxed))
	{
		wndThread.join();
		throw std::runtime_error("can't create window");
	}
}

Window::~Window()
{
	const auto threadId = GetThreadId(wndThread.native_handle());
	PostThreadMessageW(threadId, WM_QUIT, 0, 0);

	wndThread.join();
}

HWND Window::Create(HINSTANCE hinst)
{
	return CreateWindowExW(0, L"STATIC", L"C++ Header Protector Window", WS_CAPTION, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hinst, nullptr);
}

void Window::MainLoop()
{
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HWND Window::GetHwnd() const
{
	return hwnd.load(std::memory_order_relaxed);
}