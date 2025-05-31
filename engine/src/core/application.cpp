#include "application.h"
#include "core/logger.h"
#include "game_types.h"
#include "core/kiwi_mem.h"
#include "core/event.h"
#include "core/input.h"

Application *Application::Instance = nullptr;

b8 Application::Create(Game *GameInstance)
{
	if (Application::Instance)
	{
		LogError("Application::Create called more than once.");
		return false;
	}
	else
	{
		Application::Instance = (Application *)MemSystem::Allocate(sizeof(Application), MemTag_Application);
	}

	Application::Instance->GameInstance = GameInstance;

	// Initialize Subsystems
	Logger::Initialize();
	if (!EventSystem::Initialize())
	{
		LogFatal("Event system failed to initialize");
		return false;
	}
	InputSystem::Initialize();

	// TODO: Remove this
	LogFatal("This is a test message: %.2f", 3.14f);
	LogError("This is a test message: %.2f", 3.14f);
	LogWarning("This is a test message: %.2f", 3.14f);
	LogInfo("This is a test message: %.2f", 3.14f);
	LogDebug("This is a test message: %.2f", 3.14f);
	LogTrace("This is a test message: %.2f", 3.14f);

	// Flag the application as running
	Application::Instance->IsRunning = true;
	Application::Instance->IsSuspended = false;

	// Startup the platform with the informations inside the game instance
	ApplicationConfig *AppConfig = &Application::Instance->GameInstance->AppConfig;
	if (!Platform::Startup(&Application::Instance->PlatformState, AppConfig->Name,
						   AppConfig->PosX, AppConfig->PosY, AppConfig->Width, AppConfig->Height))
	{
		return false;
	}

	// Initialize the game
	if (!Application::Instance->GameInstance->Initialize(Application::Instance->GameInstance))
	{
		LogFatal("Game failed to initialize");
		return false;
	}

	Application::Instance->GameInstance->OnResize(Application::Instance->GameInstance,
												  Application::Instance->Width,
												  Application::Instance->Height);

	return true;
}

b8 Application::Run()
{
	LogInfo("%s", MemSystem::Report());

	while (Application::Instance->IsRunning)
	{
		if (!Platform::ProcessMessageQueue(&Application::Instance->PlatformState))
		{
			Instance->IsRunning = false;
		}

		if (!Application::Instance->IsSuspended)
		{
			// TODO: pass a real delta time
			if (!Application::Instance->GameInstance->Update(Application::Instance->GameInstance, 0.f))
			{
				LogFatal("Game update failed, shutting down");
				Application::Instance->IsRunning = false;
			}

			// TODO: pass a real delta time
			if (!Application::Instance->GameInstance->Render(Application::Instance->GameInstance, 0.f))
			{
				LogFatal("Game render failed, shutting down");
				Application::Instance->IsRunning = false;
			}

			// NOTE: Since the update function for the input swaps
			// the previous/current states, we want to perform this
			// at the end of the frame so that the game operates
			// on fresh input provided by the OS
			InputSystem::Update();
		}
	}

	// TODO: Check all the Terminate function to make sure
	// we actually need to terminate these subsystems or
	// we cand simply let the OS handle it for us
	InputSystem::Terminate();
	EventSystem::Terminate();
	Platform::Terminate(&Instance->PlatformState);

	return true;
}