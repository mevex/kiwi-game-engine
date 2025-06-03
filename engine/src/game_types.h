#pragma once

#include "core/application.h"

// NOTE: Forward declaration
struct Game;

#define INITIALIZE(name) b8 name(Game *GameInstance)
typedef INITIALIZE(initialize);

#define UPDATE(name) b8 name(Game *GameInstance, f32 DeltaTime)
typedef UPDATE(update);

#define RENDER(name) b8 name(Game *GameInstance, f32 DeltaTime)
typedef RENDER(render);

#define ON_RESIZE(name) void name(Game *GameInstance, u32 Width, u32 Height)
typedef ON_RESIZE(on_resize);

struct Game
{
	ApplicationConfig AppConfig;

	initialize *Initialize = nullptr;
	update *Update = nullptr;
	render *Render = nullptr;
	on_resize *OnResize = nullptr;

	// NOTE: Game state created and managed by the game
	void *State;
};