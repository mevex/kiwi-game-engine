#include "entry_point.h"
#include "game.h"
#include "core/kiwi_mem.h"

b8 CreateGame(Game *OutGame)
{
	OutGame->AppConfig.Name = "Kiwi Engine";
	OutGame->AppConfig.PosX = 200;
	OutGame->AppConfig.PosY = 200;
	OutGame->AppConfig.Width = 1280;
	OutGame->AppConfig.Height = 720;

	OutGame->Arena = MemSystem::GetArena(MemTag_Game);

	OutGame->Initialize = GameInitialize;
	OutGame->Update = GameUpdate;
	OutGame->Render = GameRender;
	OutGame->OnResize = GameOnResize;

	OutGame->State = OutGame->Arena->Push(sizeof(GameState));

	return true;
}