#include "game.h"
#include "core/logger.h"

#pragma warning(suppress : 4100)
INITIALIZE(GameInitialize)
{
	LogDebug("Game Initialize");
	return true;
}

#pragma warning(suppress : 4100)
UPDATE(GameUpdate)
{
	LogDebug("Game Update");
	return true;
}

#pragma warning(suppress : 4100)
RENDER(GameRender)
{
	LogDebug("Game Render");
	return true;
}

#pragma warning(suppress : 4100)
ON_RESIZE(GameOnResize)
{
	LogDebug("Game On Resize");
}