#pragma once

#include "defines.h"
#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"
#include "core/kiwi_mem.h"

extern b8 CreateGame(Game *OutGame);

int main(void)
{
	// NOTE: Only very specific subsystems will be initialized here
	MemSystem::Initialize();

	// Initialize the game instance
	Game GameInstance;
	if (!CreateGame(&GameInstance))
	{
		LogFatal("Could not create the game");
		return -1;
	}

	if (!GameInstance.Initialize || !GameInstance.Update || !GameInstance.Render || !GameInstance.OnResize)
	{
		LogFatal("Game's function pointers not assigned");
		return -2;
	}

	// Initialization
	if (!Application::Create(&GameInstance))
	{
		LogFatal("Could not create the application");
		return 1;
	}

	// Begin the game loop
	if (!Application::Run())
	{
		LogInfo("The application did not shut down gracefully");
		return 2;
	}

	MemSystem::Terminate();
	return 0;
}