#include "application.h"
#include "core/logger.h"
#include "game_types.h"
#include "core/kiwi_mem.h"
#include "core/event.h"
#include "core/input.h"
#include "core/timer.h"
#include "renderer/renderer_frontend.h"

ON_EVENT(HandleEvent);
ON_EVENT(HandleKey);

Application *Application::Instance = nullptr;

void Application::SetIsRunning(b8 Running)
{
	Application::Instance->IsRunning = Running;
}

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
	EventSystem::Register(SEC_ApplicationQuit, Application::Instance, HandleEvent);
	EventSystem::Register(SEC_KeyPressed, Application::Instance, HandleKey);
	EventSystem::Register(SEC_KeyReleased, Application::Instance, HandleKey);

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

	// NOTE: Initialize Renderer after the Platform in order to have a valid PlatformState
	if (!Renderer::Initialize(AppConfig->Name, &Application::Instance->PlatformState))
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
				b8 LimitFps = false;
				if (LimitFps)
				{
					Platform::SleepMS((u64)RemainingFrameTimeMS);
				}
			}
			else if (RemainingFrameTimeMS > 0.0)
			{
				LogWarning("Frame missed. Remaining Frame Time: %f ms", RemainingFrameTimeMS);
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

	InputSystem::Terminate();
	EventSystem::Terminate();
	Renderer::Terminate();
	Platform::Terminate(&Instance->PlatformState);

	return true;
}

#pragma warning(suppress : 4100)
ON_EVENT(HandleEvent)
{
	switch (Code)
	{
	case SEC_ApplicationQuit:
	{
		LogInfo("SEC_ApplicationQuit recieved, shutting down");
		Application::SetIsRunning(false);
		return true;
	}
	}
	return false;
}

#pragma warning(suppress : 4100)
ON_EVENT(HandleKey)
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