#include "platform.h"

#ifdef KIWI_WIN

#include "core/logger.h"
#include "core/input.h"

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

struct InternalState
{
	HINSTANCE InstanceHandle;
	HWND WindowHandle;
};

local_var f64 ClockFrequency;
local_var LARGE_INTEGER StartTime;

LRESULT CALLBACK
Win32ProcessMessage(HWND WindowHandle, u32 Message, WPARAM WParam, LPARAM LParam);

b8 Platform::Startup(Platform::State *PlatState, const char *ApplicationName,
					 i32 ClientX, i32 ClientY, i32 ClientWidth, i32 ClientHeight)
{
	PlatState->InternalState = malloc(sizeof(InternalState));
	InternalState *State = (InternalState *)PlatState->InternalState;

	State->InstanceHandle = GetModuleHandleA(0);

	// TODO: Set an icon for the window?
	HICON WindowIcon = LoadIcon(State->InstanceHandle, IDI_APPLICATION);
	WNDCLASSA WindowClass = {};
	WindowClass.style = (CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW);
	WindowClass.lpfnWndProc = Win32ProcessMessage;
	WindowClass.hInstance = State->InstanceHandle;
	WindowClass.hIcon = WindowIcon;
	WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
	WindowClass.lpszClassName = "Kiwi Window Class";

	if (!RegisterClassA(&WindowClass))
	{
		State = {};
		MessageBoxA(0, "Window class registration failed!", "Fatal", MB_ICONERROR | MB_OK);
		LogFatal("Window class registration failed!");
		return false;
	}

	u32 WindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	u32 WindowExtendedStyle = WS_EX_APPWINDOW;

	RECT WindowBorders = {0, 0, 0, 0};
	AdjustWindowRectEx(&WindowBorders, WindowStyle, 0, WindowExtendedStyle);

	i32 WindowX = ClientX + WindowBorders.left;
	i32 WindowY = ClientY + WindowBorders.top;
	i32 WindowWidth = ClientWidth + (WindowBorders.right - WindowBorders.left);
	i32 WindowHeight = ClientHeight + (WindowBorders.bottom - WindowBorders.top);

	State->WindowHandle =
		CreateWindowExA(WindowExtendedStyle, WindowClass.lpszClassName, ApplicationName, WindowStyle,
						WindowX, WindowY, WindowWidth, WindowHeight,
						0, 0, State->InstanceHandle, 0);
	if (!State->WindowHandle)
	{
		State = {};
		MessageBoxA(0, "Window creation failed!", "Fatal", MB_ICONERROR | MB_OK);
		LogFatal("Window creation failed!");
		return false;
	}

	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	ClockFrequency = 1.0 / (f64)Frequency.QuadPart;
	QueryPerformanceCounter(&StartTime);

	return true;
}

void Platform::Terminate(Platform::State *PlatState)
{
	InternalState *State = (InternalState *)PlatState->InternalState;

	if (State->WindowHandle)
	{
		DestroyWindow(State->WindowHandle);
		State = {};
	}
}

// NOTE: Windows does not need the platform state
#pragma warning(suppress : 4100)
b8 Platform::ProcessMessageQueue(Platform::State *PlatState)
{
	MSG Message;
	while (PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);
		DispatchMessageA(&Message);
	}

	return true;
}

// TODO: Use platform specific function (VirtualAlloc)
#pragma warning(suppress : 4100)
void *Platform::Allocate(u64 Size, b8 Alligned)
{
	return malloc(Size);
}

// TODO: Use platform specific function
#pragma warning(suppress : 4100)
void Platform::Free(void *Address, b8 Alligned)
{
	free(Address);
}

void Platform::SetMem(void *Address, u64 Size, u32 Value)
{
	// TODO: Use platform specific function
	memset(Address, Value, Size);
}

void Platform::ZeroMem(void *Address, u64 Size)
{
	// TODO: Use platform specific function
	memset(Address, 0, Size);
}

void Platform::CopyMem(void *Source, void *Dest, u64 Size)
{
	// TODO: Use platform specific function
	memcpy(Dest, Source, Size);
}

void Platform::ConsoleWrite(const char *Message, u8 Level)
{
	HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	static u8 Colors[] = {
		(BACKGROUND_RED | FOREGROUND_INTENSITY),					// Fatal
		(FOREGROUND_RED | FOREGROUND_INTENSITY),					// Error
		(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY), // Warning
		(FOREGROUND_GREEN | FOREGROUND_INTENSITY),					// Info
		(FOREGROUND_BLUE | FOREGROUND_INTENSITY),					// Debug
		(FOREGROUND_INTENSITY),										// Trace
	};
	SetConsoleTextAttribute(ConsoleHandle, Colors[Level]);

	OutputDebugStringA(Message);

	u64 Length = strlen(Message);
	LPDWORD WrittenLen = 0;
	WriteConsoleA(ConsoleHandle, Message, (DWORD)Length, WrittenLen, 0);
}

void Platform::ConsoleWriteError(const char *Message, u8 Level)
{
	HANDLE ConsoleHandle = GetStdHandle(STD_ERROR_HANDLE);
	static u8 Colors[] = {
		(BACKGROUND_RED | FOREGROUND_INTENSITY),					// Fatal
		(FOREGROUND_RED | FOREGROUND_INTENSITY),					// Error
		(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY), // Warning
		(FOREGROUND_GREEN | FOREGROUND_INTENSITY),					// Info
		(FOREGROUND_BLUE | FOREGROUND_INTENSITY),					// Debug
		(FOREGROUND_INTENSITY),										// Trace
	};
	SetConsoleTextAttribute(ConsoleHandle, Colors[Level]);

	OutputDebugStringA(Message);

	u64 Length = strlen(Message);
	LPDWORD WrittenLen = 0;
	WriteConsoleA(ConsoleHandle, Message, (DWORD)Length, WrittenLen, 0);
}

f64 Platform::GetAbsoluteTime()
{
	LARGE_INTEGER Now;
	QueryPerformanceCounter(&Now);
	return (f64)Now.QuadPart / ClockFrequency;
}

void Platform::SleepMS(u64 ms)
{
	Sleep((DWORD)ms);
}

LRESULT CALLBACK
Win32ProcessMessage(HWND WindowHandle, u32 Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;
	switch (Message)
	{
	case WM_ERASEBKGND:
	{
		// NOTE: Non-zero means that the application handle erasing background.
		// We do this to prevent flickering.
		Result = 1;
		break;
	}
	case WM_CLOSE:
	{
		// TODO: Fire an event to the application to quit
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	case WM_SIZE:
	{
		// TODO: Fire and event for window resize
		/*
		RECT R;
		GetClientRect(WindowHandle, &R);
		u32 Width = R.right - R.left;
		u32 Height = R.bottom - R.top;
		*/
		break;
	}
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		b8 Pressed = (Message == WM_KEYDOWN || Message == WM_SYSKEYDOWN);
		Key KeyCode = (Key)WParam;
		InputSystem::ProcessKey(KeyCode, Pressed);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	{
		b8 Pressed = (Message == WM_LBUTTONDOWN || Message == WM_MBUTTONDOWN || Message == WM_RBUTTONDOWN);
		MouseButton Button = MouseButton_Count;
		switch (Message)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		{
			Button = MouseButton_Left;
			break;
		}
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		{
			Button = MouseButton_Middle;
			break;
		}
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		{
			Button = MouseButton_Right;
			break;
		}
		}

		if (Button != MouseButton_Count)
			InputSystem::ProcessMouseButton(Button, Pressed);
		break;
	}
	case WM_MOUSEMOVE:
	{
		i16 XPos = (i16)GET_X_LPARAM(LParam);
		i16 YPos = (i16)GET_Y_LPARAM(LParam);
		InputSystem::ProcessMouseMove(XPos, YPos);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		// NOTE: Positive values mean that the wheel was rotated forward
		i32 ZDelta = GET_WHEEL_DELTA_WPARAM(WParam);
		i8 NumRotations = (i8)(ZDelta / WHEEL_DELTA);
		InputSystem::ProcessMouseWheel(NumRotations);
	}
	default:
	{
		// NOTE: let windows handle this code
		Result = DefWindowProc(WindowHandle, Message, WParam, LParam);
	}
	}
	return Result;
}

#endif // KIWI_WIN