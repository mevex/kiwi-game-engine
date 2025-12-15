#include "application.h"
#include "core/logger.h"
#include "game_types.h"
#include "core/kiwi_mem.h"
#include "core/input.h"
#include "core/timer.h"
#include "renderer/renderer_frontend.h"

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
		Application::Instance = (Application *)MemSystem::GetArena(MemTag_Application)->Push(sizeof(Application));
		Application::Instance->Arena = MemSystem::GetArena(MemTag_Application);
	}

	Application::Instance->GameInstance = GameInstance;

	// Initialize Subsystems
	Logger::Initialize();
	if (!EventSystem::Initialize())
	{
		LogFatal("Event system failed to initialize");
		return false;
	}
	EventSystem::Register(SEC_ApplicationQuit, Application::Instance, HandleEvent);
	EventSystem::Register(SEC_KeyPressed, Application::Instance, HandleKey);
	EventSystem::Register(SEC_KeyReleased, Application::Instance, HandleKey);
	EventSystem::Register(SEC_Resized, Application::Instance, Resize);

	InputSystem::Initialize();

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

	// NOTE: Initialize Renderer after the Platform in order to have a valid PlatformState
	if (!Renderer::Initialize(AppConfig->Name, AppConfig->Width, AppConfig->Height, &Application::Instance->PlatformState))
	{
		LogFatal("Failed to initialize the Renderer");
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
	// Time management init
	f64 TargetFrameTime = 1.0 / 60.0;
	Timer ApplicationClock;
	ApplicationClock.Start();
	ApplicationClock.Update();
	f64 LastFrameTime = ApplicationClock.UpdatedTime;

	LogInfo("%s", MemSystem::Report());

	while (Application::Instance->IsRunning)
	{
		if (!Platform::ProcessMessageQueue(&Application::Instance->PlatformState))
		{
			Instance->IsRunning = false;
		}

		if (!Application::Instance->IsSuspended)
		{
			// Update clock and get DeltaTime
			ApplicationClock.Update();
			f64 DeltaTime = (ApplicationClock.UpdatedTime - LastFrameTime);
			LastFrameTime = ApplicationClock.UpdatedTime;
			// LogDebug("Delta Time: %fms", DeltaTime * 1000);

			if (!Application::Instance->GameInstance->Update(Application::Instance->GameInstance,
															 (f32)DeltaTime))
			{
				LogFatal("Game update failed, shutting down");
				Application::Instance->IsRunning = false;
			}

			if (!Application::Instance->GameInstance->Render(Application::Instance->GameInstance,
															 (f32)DeltaTime))
			{
				LogFatal("Game render failed, shutting down");
				Application::Instance->IsRunning = false;
			}

			// TODO: temporary code! Refactor!
			RenderPacket Packet;
			Packet.DeltaTime = (f32)DeltaTime;
			Renderer::DrawFrame(&Packet);

			// calculate how much we have to sleep
			f64 ActualFrameTime = Platform::GetAbsoluteTime() - LastFrameTime;
			f64 RemainingFrameTimeMS = (TargetFrameTime - ActualFrameTime) * 1000.0;

			// NOTE: Check for > 1.0 because we cannot sleep for fractional ms
			if (RemainingFrameTimeMS > 1.0)
			{
				// TODO: Hardcoded for now
				b8 LimitFps = true;
				if (LimitFps)
				{
					Platform::SleepMS((u64)RemainingFrameTimeMS);
					RemainingFrameTimeMS -= (u64)RemainingFrameTimeMS;
				}
			}
			else if (RemainingFrameTimeMS < 0.0)
			{
				LogWarning("Frame missed. Remaining Frame Time: %f ms", RemainingFrameTimeMS);
			}

			// NOTE: Spin-lock until frame time is reached
			while (RemainingFrameTimeMS > 0.0)
			{
				ActualFrameTime = Platform::GetAbsoluteTime() - LastFrameTime;
				RemainingFrameTimeMS = (TargetFrameTime - ActualFrameTime) * 1000.0;
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
	EventSystem::Unregister(SEC_ApplicationQuit, Application::Instance, HandleEvent);
	EventSystem::Unregister(SEC_KeyPressed, Application::Instance, HandleKey);
	EventSystem::Unregister(SEC_KeyReleased, Application::Instance, HandleKey);
	EventSystem::Unregister(SEC_Resized, Application::Instance, Resize);

	InputSystem::Terminate();
	EventSystem::Terminate();
	Renderer::Terminate();
	Platform::Terminate(&Instance->PlatformState);

	return true;
}

SUPPRESS_WARNING(4100)
DEFINE_EVENT_FUNCTION(Application::HandleEvent)
{
	switch (Code)
	{
	case SEC_ApplicationQuit:
	{
		LogInfo("SEC_ApplicationQuit recieved, shutting down");
		Instance->IsRunning = false;
		return true;
	}
	}
	return false;
}

SUPPRESS_WARNING(4100)
DEFINE_EVENT_FUNCTION(Application::HandleKey)
{
	if (Code == SEC_KeyReleased)
	{
		u16 KeyCode = Data.u16[0];
		if ((KeyCode == Key_F4 && InputSystem::IsKeyDown(Key_Alt)) ||
			KeyCode == Key_Escape)
		{
			// NOTE: Escape is a convenience way to quickly quit the application
			// TODO: It won't stay here
			EventSystem::Fire(SEC_ApplicationQuit, nullptr, {});
			return true;
		}
	}
	return false;
}

SUPPRESS_WARNING(4100)
DEFINE_EVENT_FUNCTION(Application::Resize)
{
	u16 Width = Data.u16[0];
	u16 Height = Data.u16[1];

	ApplicationConfig *AppConfig = &Instance->GameInstance->AppConfig;
	if (Width != AppConfig->Width || Height != AppConfig->Height)
	{
		// TODO: WTF is with all these duplicate infos????
		AppConfig->Width = Width;
		AppConfig->Height = Height;
		Instance->Width = Width;
		Instance->Height = Height;

		LogDebug("Window Resized: %i, %i", Width, Height);

		// Handle minimization
		if (Width == 0 || Height == 0)
		{
			LogInfo("Window minimized, suspending application");
			Instance->IsSuspended = true;
		}
		else
		{
			Instance->IsSuspended = false;
			Instance->GameInstance->OnResize(Instance->GameInstance, Width, Height);
			Renderer::OnResized(Width, Height);
		}
	}

	// NOTE: Event handled! I don't want anybody to mess with this
	// and all the code should be in one place for the engine: here,
	// and one place for the game: Game::OnResize
	return true;
}