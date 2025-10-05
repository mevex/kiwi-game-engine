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

	for (u16 Index = 0; Index < MAX_MESSAGE_CODES; ++Index)
	{
		Registered[Index].Events.Create(MemTag_EventSystem);
	}
	IsInitialized = true;
	return true;
}

b8 EventSystem::Terminate()
{
	// TODO: Do we actually need to deinitialize this?
	for (u16 Index = 0; Index < MAX_MESSAGE_CODES; ++Index)
	{
		if (Registered[Index].Events.FirstNode || Registered[Index].Events.FreeNode)
		{
			// TODO: This is theoretically wrong since we are not necesarily freeing from
			// the correct linked lists. I.E. LLA frees into LLB space and vice versa.
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

	KLinkedList<RegisteredEvent> *Events = &Registered[Code].Events;

	for (KLinkedList<RegisteredEvent>::Node *Node = Events->FirstNode; Node; Node = Node->Next)
	{
		if (Node->Element.Listener == Listener)
		{
			LogWarning("Listener arleady registered to code %d", Code);
			return false;
		}
	}

	RegisteredEvent Event;
	Event.Listener = Listener;
	Event.Callback = OnEvent;
	Events->Add(Event);

	return true;
}

b8 EventSystem::Unregister(u16 Code, void *Listener, on_event OnEvent)
{
	if (!IsInitialized)
	{
		LogError("Event System not yet initialized. Event unregister aborted");
		return false;
	}

	KLinkedList<RegisteredEvent> *Events = &Registered[Code].Events;

	for (KLinkedList<RegisteredEvent>::Node *Node = Events->FirstNode; Node; Node = Node->Next)
	{
		if (Node->Element.Listener == Listener && Node->Element.Callback == OnEvent)
		{
			Events->Remove(Node);
			return true;
		}
	}

	// Event not found
	LogWarning("Listener not registered for event %d", Code);
	return false;
}

b8 EventSystem::Fire(u16 Code, void *Sender, EventContext Context)
{
	if (!IsInitialized)
	{
		LogError("Event System not yet initialized. Cannot fire an event");
		return false;
	}

	KLinkedList<RegisteredEvent> *Events = &Registered[Code].Events;

	for (KLinkedList<RegisteredEvent>::Node *Node = Events->FirstNode; Node; Node = Node->Next)
	{
		if (Node->Element.Callback(Code, Sender, Node->Element.Listener, Context))
		{
			// NOTE: if we enter here it means that the event has been handled
			// and there is no need to send it to someone else
			return true;
		}
	}

	return false;
}