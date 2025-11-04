#include "platform.h"

#ifdef KIWI_WIN

#include "core/logger.h"
#include "core/input.h"
#include "core/event.h"
#include "core/kiwi_mem.h"

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>

#define SCHEDULER_GRANULARITY 1

struct InternalState
{
	HINSTANCE InstanceHandle;
	HWND WindowHandle;
};

local_var f64 ClockFrequency;
local_var LARGE_INTEGER StartTime;

LRESULT CALLBACK
Win32ProcessMessage(HWND WindowHandle, u32 Message, WPARAM WParam, LPARAM LParam);

b8 Platform::Startup(PlatformState *PlatState, const char *ApplicationName,
					 i32 ClientX, i32 ClientY, i32 ClientWidth, i32 ClientHeight)
{
	// TODO: Replace malloc
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

	// NOTE: Set the Windows scheduler granularity to 1 ms
	// so that Sleep() can be more granular.
	// Returns TIMERR_NOCANDO if it fails
	if (timeBeginPeriod(SCHEDULER_GRANULARITY) != TIMERR_NOERROR)
	{
		LogFatal("Could not set the scheduler granularity");
		return false;
	}

	return true;
}

void Platform::Terminate(PlatformState *PlatState)
{
	// NOTE: Reset the scheduler granlarity
	timeEndPeriod(SCHEDULER_GRANULARITY);

	InternalState *State = (InternalState *)PlatState->InternalState;

	if (State->WindowHandle)
	{
		DestroyWindow(State->WindowHandle);
		State = {};
	}
}

// NOTE: Windows does not need the platform state
SUPPRESS_WARNING(4100)
b8 Platform::ProcessMessageQueue(PlatformState *PlatState)
{
	MSG Message;
	while (PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);
		DispatchMessageA(&Message);
	}

	return true;
}

char *Platform::GetLastErrorMessage()
{
	// TODO: This function is currently LEAKING MEMORY
	u32 Error = GetLastError();

	LPSTR ErrorMessage = 0;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				   NULL, Error, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPSTR)&ErrorMessage,
				   0, NULL);

	return (char *)ErrorMessage;
}

void Platform::GetMemoryInfo(u32 &OutPageSize, u32 &OutAllocationGranularity)
{
	// Retrieve the system informations
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	OutPageSize = (u32)SystemInfo.dwPageSize;
	OutAllocationGranularity = (u32)SystemInfo.dwAllocationGranularity;
};

char *Platform::GetMemoryAllocationInfo(void *Address)
{
	char Buffer[1000] = {};

	MEMORY_BASIC_INFORMATION MemInfo;
	if (VirtualQuery(Address, &MemInfo, sizeof(MemInfo)))
	{
		const char *States[3] = {
			"MEM_COMMIT",
			"MEM_FREE",
			"MEM_RESERVE",
		};
		const char *Protections[2] = {
			"PAGE_NOACCESS",
			"PAGE_READWRITE",
		};
		const char *State = States[MemInfo.State == 0x1000 ? 0 : MemInfo.State == 0x10000 ? 1
																						  : 2];
		const char *Protection = Protections[MemInfo.Protect == 0x1 ? 0 : 1];

		snprintf(Buffer, 1000,
				 "Allocation info for Address 0x%llx\nBaseAddress=0x%llx, AllocationBase=0x%llx, RegionSize=%llu, State=%s, Protect=%s",
				 (u64)Address, (u64)MemInfo.BaseAddress, (u64)MemInfo.AllocationBase, (u64)MemInfo.RegionSize, State, Protection);
	}

	// TODO: This function is currently LEAKING MEMORY
	return _strdup(Buffer);
}

void Platform::TranslateAllocSpecifiers(u32 MemAllocFlags, u32 &OutAllocType, u32 &OutProtectionType)
{
	// Allocation types
	if (CheckFlags(MemAllocFlags, MemAlloc_Commit))
	{
		OutAllocType |= MEM_COMMIT;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_Reserve))
	{
		OutAllocType |= MEM_RESERVE;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_Reset))
	{
		OutAllocType |= MEM_RESET;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_ResetUndo))
	{
		OutAllocType |= MEM_RESET_UNDO;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_LargePages))
	{
		OutAllocType |= MEM_LARGE_PAGES;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_Physical))
	{
		OutAllocType |= MEM_PHYSICAL;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_TopDown))
	{
		OutAllocType |= MEM_TOP_DOWN;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_WriteWatch))
	{
		OutAllocType |= MEM_WRITE_WATCH;
	}

	// Protection Type
	if (CheckFlags(MemAllocFlags, MemAlloc_Execute))
	{
		OutProtectionType |= PAGE_EXECUTE;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_ExecuteRead))
	{
		OutProtectionType |= PAGE_EXECUTE_READ;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_ExecuteReadWrite))
	{
		OutProtectionType |= PAGE_EXECUTE_READWRITE;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_ExecuteWriteCopy))
	{
		OutProtectionType |= PAGE_EXECUTE_WRITECOPY;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_NoAccess))
	{
		OutProtectionType |= PAGE_NOACCESS;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_ReadOnly))
	{
		OutProtectionType |= PAGE_READONLY;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_ReadWrite))
	{
		OutProtectionType |= PAGE_READWRITE;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_WriteCopy))
	{
		OutProtectionType |= PAGE_WRITECOPY;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_Guard))
	{
		OutProtectionType |= PAGE_GUARD;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_NoCache))
	{
		OutProtectionType |= PAGE_NOCACHE;
	}
	if (CheckFlags(MemAllocFlags, MemAlloc_WriteCombine))
	{
		OutProtectionType |= PAGE_WRITECOMBINE;
	}
}

void *Platform::Allocate(void *Address, u64 Size, u32 MemAllocFlags)
{
	u32 AllocationType = 0;
	u32 ProtectionType = 0;
	TranslateAllocSpecifiers(MemAllocFlags, AllocationType, ProtectionType);

	return VirtualAlloc(Address, Size, AllocationType, ProtectionType);
}

void Platform::Free(void *Address, u64 Size, u8 MemDeallocFlag)
{
	b8 IsRelease = CheckFlags(MemDeallocFlag, MemDealloc_Release);
	VirtualFree(Address,
				IsRelease ? 0 : Size,
				IsRelease ? MEM_RELEASE : MEM_DECOMMIT);
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
	// TODO: rework all the time stuff to be able to work with f32 safely
	LARGE_INTEGER Now;
	QueryPerformanceCounter(&Now);
	return (f64)Now.QuadPart * ClockFrequency;
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
		EventSystem::Fire(SEC_ApplicationQuit, nullptr, {});
		break;
	}
	case WM_DESTROY:
	{
		LogInfo("WM Destroy");
		PostQuitMessage(0);
		break;
	}
	case WM_SIZE:
	{
		RECT R;
		GetClientRect(WindowHandle, &R);
		u32 Width = R.right - R.left;
		u32 Height = R.bottom - R.top;

		EventContext Context = {};
		Context.u16[0] = (u16)Width;
		Context.u16[1] = (u16)Height;
		EventSystem::Fire(SEC_Resized, nullptr, Context);
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
		i8 NumRotations = (i8)(Clamp(ZDelta, -1, 1));
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

// Vulkan-Win32 Platform Specific
// TODO: Maybe split this?
#include "renderer/vulkan/vulkan_platform.h"
#include "renderer/vulkan/vulkan_types.h"
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_win32.h"

void VulkanPlatform::GetExtensions(KArray<const char *> &Extensions)
{
	Extensions.Push(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
}

b8 VulkanPlatform::CreateSurface(PlatformState *PlatState, VulkanContext *VkContext)
{
	InternalState *State = (InternalState *)PlatState->InternalState;

	VkWin32SurfaceCreateInfoKHR CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	CreateInfo.hinstance = State->InstanceHandle;
	CreateInfo.hwnd = State->WindowHandle;

	VkResult Result = vkCreateWin32SurfaceKHR(VkContext->Instance, &CreateInfo,
											  VkContext->Allocator, &VkContext->Surface);
	if (Result != VK_SUCCESS)
	{
		LogFatal("Win32 Vulkan surface creation failed");
		return false;
	}

	LogInfo("Win32 Vulkan surface created successfully");
	return true;
}

#endif // KIWI_WIN