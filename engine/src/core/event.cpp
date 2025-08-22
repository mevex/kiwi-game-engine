#include "event.h"
#include "core/kiwi_mem.h"
#include "core/logger.h"
#include "containers/karray.h"

EventCodeEntry EventSystem::Registered[MAX_MESSAGE_CODES];
b8 EventSystem::IsInitialized = false;

b8 EventSystem::Initialize()
{
	if (IsInitialized)
	{
		LogError("EventSystem already initialized");
		return false;
	}

	MemSystem::Zero(Registered, MAX_MESSAGE_CODES * sizeof(EventCodeEntry));
	IsInitialized = true;
	return true;
}

b8 EventSystem::Terminate()
{
	for (u16 Index = 0; Index < MAX_MESSAGE_CODES; ++Index)
	{
		if (Registered[Index].Events.Length)
		{
			Registered[Index].Events.Destroy();
		}
	}

	return true;
}

b8 EventSystem::Register(u16 Code, void *Listener, on_event OnEvent)
{
	if (!IsInitialized)
	{
		LogError("Event System not yet initialized. Event registration aborted");
		return false;
	}

	KArray<RegisteredEvent> *Events = &Registered[Code].Events;

	for (u64 Index = 0; Index < Events->Length; ++Index)
	{
		if ((*Events)[Index].Listener == Listener)
		{
			LogWarning("Listener arleady registered to code %d", Code);
			return false;
		}
	}

	RegisteredEvent Event;
	Event.Listener = Listener;
	Event.Callback = OnEvent;
	Events->Push(Event);

	return true;
}

b8 EventSystem::Unregister(u16 Code, void *Listener, on_event OnEvent)
{
	if (!IsInitialized)
	{
		LogError("Event System not yet initialized. Event unregister aborted");
		return false;
	}

	KArray<RegisteredEvent> *Events = &Registered[Code].Events;
	if (Events->Capacity == 0)
	{
		return false;
	}

	for (u64 Index = 0; Index < Events->Length; ++Index)
	{
		RegisteredEvent Event = (*Events)[Index];
		if (Event.Listener == Listener && Event.Callback == OnEvent)
		{
			Events->RemoveAt(Index);
			return true;
		}
	}

	// Event not found
	return false;
}

b8 EventSystem::Fire(u16 Code, void *Sender, EventContext Context)
{
	if (!IsInitialized)
	{
		LogError("Event System not yet initialized. Cannot fire an event");
		return false;
	}

	KArray<RegisteredEvent> *Events = &Registered[Code].Events;

	for (u64 Index = 0; Index < Events->Length; ++Index)
	{
		RegisteredEvent Event = (*Events)[Index];
		if (Event.Callback(Code, Sender, Event.Listener, Context))
		{
			// NOTE: if we enter here it means that the event has been handled
			// and there is no need to send it to someone else
			return true;
		}
	}

	return false;
}