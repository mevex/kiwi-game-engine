#pragma once

#include "defines.h"

enum MouseButton : u8
{
	// TODO: Add more mouse buttons
	MouseButton_Left,
	MouseButton_Middle,
	MouseButton_Right,

	MouseButton_Count
};

#define DEFINE_KEY(name, code) Key_##name = code

// NOTE: These are the key codes used in Windows
enum Key : u16
{
	DEFINE_KEY(Backspace, 0x08),
	DEFINE_KEY(Enter, 0x0D),
	DEFINE_KEY(Tab, 0x09),
	DEFINE_KEY(Shift, 0x10),
	DEFINE_KEY(Control, 0x11),

	DEFINE_KEY(Pause, 0x13),
	DEFINE_KEY(Capital, 0x14),

	DEFINE_KEY(Escape, 0x1B),

	DEFINE_KEY(Convert, 0x1C),
	DEFINE_KEY(NonConvert, 0x1D),
	DEFINE_KEY(Accept, 0x1E),
	DEFINE_KEY(ModeChange, 0x1F),

	DEFINE_KEY(Space, 0x20),
	DEFINE_KEY(Priot, 0x21),
	DEFINE_KEY(Next, 0x22),
	DEFINE_KEY(End, 0x23),
	DEFINE_KEY(Home, 0x24),
	DEFINE_KEY(Left, 0x25),
	DEFINE_KEY(Up, 0x26),
	DEFINE_KEY(Right, 0x27),
	DEFINE_KEY(Down, 0x28),
	DEFINE_KEY(Select, 0x29),
	DEFINE_KEY(Print, 0x2A),
	DEFINE_KEY(Execute, 0x2B),
	DEFINE_KEY(Snapshot, 0x2C),
	DEFINE_KEY(Insert, 0x2D),
	DEFINE_KEY(Delete, 0x2E),
	DEFINE_KEY(Help, 0x2F),

	DEFINE_KEY(A, 0x41),
	DEFINE_KEY(B, 0x42),
	DEFINE_KEY(C, 0x43),
	DEFINE_KEY(D, 0x44),
	DEFINE_KEY(E, 0x45),
	DEFINE_KEY(F, 0x46),
	DEFINE_KEY(G, 0x47),
	DEFINE_KEY(H, 0x48),
	DEFINE_KEY(I, 0x49),
	DEFINE_KEY(J, 0x4A),
	DEFINE_KEY(K, 0x4B),
	DEFINE_KEY(L, 0x4C),
	DEFINE_KEY(M, 0x4D),
	DEFINE_KEY(N, 0x4E),
	DEFINE_KEY(O, 0x4F),
	DEFINE_KEY(P, 0x50),
	DEFINE_KEY(Q, 0x51),
	DEFINE_KEY(R, 0x52),
	DEFINE_KEY(S, 0x53),
	DEFINE_KEY(T, 0x54),
	DEFINE_KEY(U, 0x55),
	DEFINE_KEY(V, 0x56),
	DEFINE_KEY(W, 0x57),
	DEFINE_KEY(X, 0x58),
	DEFINE_KEY(Y, 0x59),
	DEFINE_KEY(Z, 0x5A),

	DEFINE_KEY(LWin, 0x5B),
	DEFINE_KEY(RWin, 0x5C),
	DEFINE_KEY(Apps, 0x5D),

	DEFINE_KEY(Sleep, 0x5F),

	DEFINE_KEY(Numpad0, 0x60),
	DEFINE_KEY(Numpad1, 0x61),
	DEFINE_KEY(Numpad2, 0x62),
	DEFINE_KEY(Numpad3, 0x63),
	DEFINE_KEY(Numpad4, 0x64),
	DEFINE_KEY(Numpad5, 0x65),
	DEFINE_KEY(Numpad6, 0x66),
	DEFINE_KEY(Numpad7, 0x67),
	DEFINE_KEY(Numpad8, 0x68),
	DEFINE_KEY(Numpad9, 0x69),
	DEFINE_KEY(Multiply, 0x6A),
	DEFINE_KEY(Add, 0x6B),
	DEFINE_KEY(Separator, 0x6C),
	DEFINE_KEY(Subtract, 0x6D),
	DEFINE_KEY(Decimal, 0x6E),
	DEFINE_KEY(Divide, 0x6F),
	DEFINE_KEY(F1, 0x70),
	DEFINE_KEY(F2, 0x71),
	DEFINE_KEY(F3, 0x72),
	DEFINE_KEY(F4, 0x73),
	DEFINE_KEY(F5, 0x74),
	DEFINE_KEY(F6, 0x75),
	DEFINE_KEY(F7, 0x76),
	DEFINE_KEY(F8, 0x77),
	DEFINE_KEY(F9, 0x78),
	DEFINE_KEY(F10, 0x79),
	DEFINE_KEY(F11, 0x7A),
	DEFINE_KEY(F12, 0x7B),
	DEFINE_KEY(F13, 0x7C),
	DEFINE_KEY(F14, 0x7D),
	DEFINE_KEY(F15, 0x7E),
	DEFINE_KEY(F16, 0x7F),
	DEFINE_KEY(F17, 0x80),
	DEFINE_KEY(F18, 0x81),
	DEFINE_KEY(F19, 0x82),
	DEFINE_KEY(F20, 0x83),
	DEFINE_KEY(F21, 0x84),
	DEFINE_KEY(F22, 0x85),
	DEFINE_KEY(F23, 0x86),
	DEFINE_KEY(F24, 0x87),

	DEFINE_KEY(NumLock, 0x90),
	DEFINE_KEY(Scroll, 0x91),

	DEFINE_KEY(NumpadEqual, 0x92),

	DEFINE_KEY(LShift, 0xA0),
	DEFINE_KEY(RShift, 0xA1),
	DEFINE_KEY(LControl, 0xA2),
	DEFINE_KEY(RControl, 0xA3),
	DEFINE_KEY(LMenu, 0xA4),
	DEFINE_KEY(RMenu, 0xA5),

	DEFINE_KEY(Semicolor, 0xBA),
	DEFINE_KEY(Plus, 0xBB),
	DEFINE_KEY(Comma, 0xBC),
	DEFINE_KEY(Minus, 0xBD),
	DEFINE_KEY(Period, 0xBE),
	DEFINE_KEY(Slash, 0xBF),
	DEFINE_KEY(Grave, 0xC0),

	Key_Count
};

struct KeyboardState
{
	b8 Keys[Key_Count];
};

struct MouseState
{
	b8 Buttons[MouseButton_Count];
	i16 X;
	i16 Y;
};

class InputSystem
{
	// TODO: Add gamepad support
public:
	static void Initialize();
	static void Terminate();
	static void Update();

	// Keyboard
	static void ProcessKey(Key KeyCode, b8 Pressed);

	KIWI_API static b8 IsKeyDown(Key KeyCode);
	KIWI_API static b8 IsKeyUp(Key KeyCode);
	KIWI_API static b8 WasKeyDown(Key KeyCode);
	KIWI_API static b8 WasKeyUp(Key KeyCode);

	// Mouse
	static void ProcessMouseButton(MouseButton Button, b8 Pressed);
	static void ProcessMouseMove(i16 X, i16 Y);
	static void ProcessMouseWheel(i8 ZDelta);

	KIWI_API static b8 IsMouseButtonDown(MouseButton Button);
	KIWI_API static b8 IsMouseButtonUp(MouseButton Button);
	KIWI_API static b8 WasMouseButtonDown(MouseButton Button);
	KIWI_API static b8 WasMouseButtonUp(MouseButton Button);
	KIWI_API static void GetMousePosition(i16 &X, i16 &Y);
	KIWI_API static void GetPreviousMousePosition(i16 &X, i16 &Y);
	KIWI_API static i8 GetMouseWheelDelta();

private:
	static b8 IsInitialized;

	static KeyboardState KState[2];
	static MouseState MState[2];
	static i8 MouseWheelZ;

	static KeyboardState *CurrentKState;
	static KeyboardState *PreviousKState;

	static MouseState *CurrentMState;
	static MouseState *PreviousMState;
};