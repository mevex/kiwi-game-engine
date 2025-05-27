#include "game.h"
#include "core/logger.h"

#pragma warning(suppress : 4100)
INITIALIZE(GameInitialize)
{
	return true;
}

#pragma warning(suppress : 4100)
UPDATE(GameUpdate)
{
	return true;
}

#pragma warning(suppress : 4100)
RENDER(GameRender)
{
	return true;
}

#pragma warning(suppress : 4100)
ON_RESIZE(GameOnResize)
{
}