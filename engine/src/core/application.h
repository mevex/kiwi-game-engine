#pragma once

#include "defines.h"
#include "platform/platform.h"
#include "core/event.h"

struct Game;
class MemArena;

struct ApplicationConfig
{
	const char *Name;
	u16 PosX;
	u16 PosY;
	u16 Width;
	u16 Height;
};

// NOTE: this is a singleton
class Application
{
public:
	KIWI_API static b8 Create(Game *GameInstance);
	KIWI_API static b8 Run();

	// Events
	static DEFINE_EVENT_FUNCTION(HandleEvent);
	static DEFINE_EVENT_FUNCTION(HandleKey);
	static DEFINE_EVENT_FUNCTION(Resize);

private:
	static Application *Instance;

	MemArena *Arena = nullptr;
	Game *GameInstance;
	b8 IsRunning = false;
	b8 IsSuspended = false;
	PlatformState PlatformState;
	u16 Width;
	u16 Height;
};