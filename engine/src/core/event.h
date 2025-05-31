#pragma once

#include "defines.h"
#include "containers/karray.h"

struct EventContext
{
	union
	{
		i64 i64[2];
		u64 u64[2];
		f64 f64[2];

		i32 i32[4];
		u32 u32[4];
		f32 f32[4];

		i16 i16[8];
		u16 u16[8];

		i8 i8[16];
		u8 u8[16];

		char c[16];
	};
};

// NOTE: Application defined event code will start at 256 (0x100)
enum SystemEventCode : u8
{
	// Shuts down the application on the next frame.
	SEC_ApplicationQuit = 0x01,

	// Keyboard key pressed
	// Context usage:
	// u16[0] = KeyCode
	SEC_KeyPressed = 0x02,

	// Keyboard key released
	// Context usage:
	// u16[0] = KeyCode
	SEC_KeyReleased = 0x03,

	// Mouse button pressed
	// Context usage:
	// u16[0] = MouseButton
	SEC_MouseButtonPressed = 0x04,

	// Mouse button released
	// Context usage:
	// u16[0] = MouseButton
	SEC_MouseButtonReleased = 0x05,

	// Mouse moved
	// Context usage:
	// i16[0] = X
	// i16[1] = Y
	SEC_MouseMoved = 0x06,

	// Mouse wheel rotated
	// Context usage:
	// i8[0] = ZDelta
	SEC_MouseWheel = 0x07,

	// Resized/resolution changed
	// Context usage:
	// u16[0] = Width
	// u16[1] = Height
	SEC_Resized = 0x08,

	SEC_Max = 0xff
};

#define ON_EVENT(name) b8 name(u16 Code, void *Sender, void *Listener, EventContext Data)
typedef ON_EVENT(on_event);

struct RegisteredEvent
{
	void *Listener;
	on_event *Callback;
};

struct EventCodeEntry
{
	KArray<RegisteredEvent> Events;
};

#define MAX_MESSAGE_CODES 4096

class EventSystem
{
public:
	static b8 Initialize();
	static b8 Terminate();

	// NOTE: Register to an event with the corresponding code.
	// Duplicate registration will not be performed and this will
	// return false.
	KIWI_API static b8 Register(u16 Code, void *Listener, on_event OnEvent);

	// NOTE: Unregister from an event with the provided code.
	// If no matching registration is found, this will return false.
	KIWI_API static b8 Unregister(u16 Code, void *Listener, on_event OnEvent);

	// NOTE: Fire the event to listeners of the corresponding code.
	// if an event handler returns true the event is considered handled
	// and not passed on to any more listeners.
	KIWI_API static b8 Fire(u16 Code, void *Sender, EventContext Context);

private:
	// NOTE: LUT for event codes
	static EventCodeEntry Registered[MAX_MESSAGE_CODES];
	static b8 IsInitialized;
};
