#include <Windows.h>
#include <WebView2.h>
#include <wrl.h>

#include "Definitions.h"
#include "FileSystem.h"

using namespace Microsoft::WRL;

constexpr int WINDOW_STARTING_WIDTH = 800;
constexpr int WINDOW_STARTING_HEIGHT = 600;

ComPtr<ICoreWebView2Controller> _webviewController;
ComPtr<ICoreWebView2> _webview;

static void SetWebViewSizeToMatchWindowSize(const HWND windowHandle)
{
	if (_webviewController)
	{
		RECT bounds;
		GetClientRect(windowHandle, &bounds);
		_webviewController->put_Bounds(bounds);
	}
}

static LRESULT CALLBACK ProcessWindowMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(windowHandle, &ps);
		EndPaint(windowHandle, &ps);
		return 0;
	}
	case WM_SIZE:
		SetWebViewSizeToMatchWindowSize(windowHandle);
		break;

	case WM_CLOSE:
		DestroyWindow(windowHandle);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(windowHandle, message, wParam, lParam);
}

static void RegisterWindowClass(const HINSTANCE instanceHandle)
{
	WNDCLASS windowClass = {};

	windowClass.lpfnWndProc = ProcessWindowMessage;
	windowClass.hInstance = instanceHandle;
	windowClass.lpszClassName = APPLICATION_NAME;
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&windowClass);
}

static HWND InitializeWindow(const HINSTANCE instanceHandle)
{
	HWND windowHandle = CreateWindowEx(
		0,
		APPLICATION_NAME,
		APPLICATION_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_STARTING_WIDTH,
		WINDOW_STARTING_HEIGHT,
		nullptr,
		nullptr,
		instanceHandle,
		nullptr
	);

	return windowHandle;
}

static void UseWebView(const HWND windowHandle)
{
	CreateCoreWebView2EnvironmentWithOptions(
		nullptr,
		nullptr,
		nullptr,
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[windowHandle](HRESULT result, ICoreWebView2Environment* env) -> HRESULT
			{
				env->CreateCoreWebView2Controller(
					windowHandle,
					Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
						[windowHandle](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT
						{
							if (controller)
							{
								_webviewController = controller;
								_webviewController->get_CoreWebView2(&_webview);

								SetWebViewSizeToMatchWindowSize(windowHandle);

								std::wstring executableDirectoryPath =
									FileSystem::GetExecutableDirectoryPath();

								std::wstring localHtmlFileUri =
									L"file:///" + executableDirectoryPath + HTML_FILE_NAME;

								//_webview->Navigate(L"https://www.google.com");
								_webview->Navigate(
									localHtmlFileUri.c_str());
							}
							return S_OK;
						}
					).Get()
				);
				return S_OK;
			}
		).Get()
	);
}

static void InitializeApplication(const HINSTANCE instanceHandle)
{
	(void)CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	RegisterWindowClass(instanceHandle);
}

static void ShutdownApplication(const HINSTANCE instanceHandle)
{
	CoUninitialize();
	UnregisterClass(APPLICATION_NAME, instanceHandle);
}

int WINAPI wWinMain(
	_In_ HINSTANCE instanceHandle,
	_In_opt_ HINSTANCE previousInstanceHandle,
	_In_ PWSTR arguments,
	_In_ int windowState)
{
	InitializeApplication(instanceHandle);

	HWND windowHandle = InitializeWindow(instanceHandle);

	UseWebView(windowHandle);
	ShowWindow(windowHandle, windowState);
	UpdateWindow(windowHandle);

	MSG message = {};
	while (GetMessage(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	ShutdownApplication(instanceHandle);
	return 0;
}