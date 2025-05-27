#pragma once

#include "defines.h"
#include "game_types.h"

struct GameState
{
	f32 DeltaTime;
};

INITIALIZE(GameInitialize);

UPDATE(GameUpdate);

RENDER(GameRender);

ON_RESIZE(GameOnResize);