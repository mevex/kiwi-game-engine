#include "entry_point.h"
#include "game.h"

// TODO: Remove this
#include "platform/platform.h"

b8 CreateGame(Game *OutGame)
{
	OutGame->AppConfig.Name = "Kiwi Engine";
	OutGame->AppConfig.PosX = 200;
	OutGame->AppConfig.PosY = 200;
	OutGame->AppConfig.Width = 1280;
	OutGame->AppConfig.Height = 720;

	OutGame->Initialize = GameInitialize;
	OutGame->Update = GameUpdate;
	OutGame->Render = GameRender;
	OutGame->OnResize = GameOnResize;

	OutGame->State = Platform::Allocate(sizeof(GameState));

	return true;
}