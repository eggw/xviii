#pragma once

#include "xviii/GameStates/GameState_MenuState.h"

#include "boost/filesystem/fstream.hpp"

#include "xviii/Headers/global.h"
#include "xviii/Headers/keybinds.h"

class GameState_MultiplayerMenu : public GameState_MenuState
{
public:
	GameState_MultiplayerMenu(Game* game);

	void init();
};

