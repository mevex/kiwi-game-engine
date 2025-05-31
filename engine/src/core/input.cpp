#include "input.h"
#include "core/event.h"
#include "core/kiwi_mem.h"
#include "core/logger.h"

b8 InputSystem::IsInitialized = false;

KeyboardState InputSystem::KState[2] = {};
MouseState InputSystem::MState[2] = {};
i8 InputSystem::MouseWheelZ = 0;

KeyboardState *InputSystem::CurrentKState = &KState[0];
KeyboardState *InputSystem::PreviousKState = &KState[1];

MouseState *InputSystem::CurrentMState = &MState[0];
MouseState *InputSystem::PreviousMState = &MState[1];

void InputSystem::Initialize()
{
	// TODO: Do we really need this?
	IsInitialized = true;
	LogInfo("Input subsystem initialize");
}

void InputSystem::Terminate()
{
	IsInitialized = false;
}

void InputSystem::Update()
{
	if (!IsInitialized)
	{
		LogError("Input system not initialized. Cannot update it's state");
		return;
	}

	Swap(CurrentKState, PreviousKState);
	Swap(CurrentMState, PreviousMState);
	MouseWheelZ = 0;
}

void InputSystem::ProcessKey(Key KeyCode, b8 Pressed)
{
	if (CurrentKState->Keys[KeyCode] != Pressed)
	{
		// Update internal state
		CurrentKState->Keys[KeyCode] = Pressed;

		// Fire an event
		EventContext Context;
		Context.u16[0] = (u16)KeyCode;
		EventSystem::Fire(Pressed ? SEC_KeyPressed : SEC_KeyReleased, nullptr, Context);
	}
}

b8 InputSystem::IsKeyDown(Key KeyCode)
{
	return CurrentKState->Keys[KeyCode];
}

b8 InputSystem::IsKeyUp(Key KeyCode)
{
	return !CurrentKState->Keys[KeyCode];
}

b8 InputSystem::WasKeyDown(Key KeyCode)
{
	return PreviousKState->Keys[KeyCode];
}

b8 InputSystem::WasKeyUp(Key KeyCode)
{
	return !PreviousKState->Keys[KeyCode];
}

void InputSystem::ProcessMouseButton(MouseButton Button, b8 Pressed)
{
	if (CurrentMState->Buttons[Button] != Pressed)
	{
		// Update internal state
		CurrentMState->Buttons[Button] = Pressed;

		// Fire an event
		EventContext Context;
		Context.u16[0] = (u16)Button;
		EventSystem::Fire(Pressed ? SEC_MouseButtonPressed : SEC_MouseButtonReleased, nullptr, Context);
	}
}

void InputSystem::ProcessMouseMove(i16 X, i16 Y)
{
	if (CurrentMState->X != X || CurrentMState->Y != Y)
	{
		// LogDebug("Mouse pos: %d, %d", X, Y);

		// Update internal state
		CurrentMState->X = X;
		CurrentMState->Y = Y;

		// Fire an event
		EventContext Context;
		Context.i16[0] = (i16)X;
		Context.i16[1] = (i16)Y;
		EventSystem::Fire(SEC_MouseMoved, nullptr, Context);
	}
}

void InputSystem::ProcessMouseWheel(i8 ZDelta)
{
	// Update internal state
	MouseWheelZ = ZDelta;

	// Fire an event
	EventContext Context;
	Context.i8[0] = (i8)ZDelta;
	EventSystem::Fire(SEC_MouseWheel, nullptr, Context);
}

b8 InputSystem::IsMouseButtonDown(MouseButton Button)
{
	return CurrentMState->Buttons[Button];
}

b8 InputSystem::IsMouseButtonUp(MouseButton Button)
{
	return !CurrentMState->Buttons[Button];
}

b8 InputSystem::WasMouseButtonDown(MouseButton Button)
{
	return PreviousMState->Buttons[Button];
}

b8 InputSystem::WasMouseButtonUp(MouseButton Button)
{
	return !PreviousMState->Buttons[Button];
}

void InputSystem::GetMousePosition(i16 &X, i16 &Y)
{
	X = CurrentMState->X;
	Y = CurrentMState->Y;
}

void InputSystem::GetPreviousMousePosition(i16 &X, i16 &Y)
{
	X = PreviousMState->X;
	Y = PreviousMState->Y;
}

i8 InputSystem::GetMouseWheelDelta()
{
	return MouseWheelZ;
}