#pragma once

#include "defines.h"
#include "platform/platform.h"

struct Game;

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

	static void SetIsRunning(b8 Running);

private:
	static Application *Instance;

	Game *GameInstance;
	b8 IsRunning = false;
	b8 IsSuspended = false;
	PlatformState PlatformState;
	u16 Width;
	u16 Height;
};