#include "xviii/Headers/stdafx.h"
#include "xviii/GameStates/GameState_CustomBattleLoadMenu.h"

#include "xviii/Core/Game.h"

GameState_CustomBattleLoadMenu::GameState_CustomBattleLoadMenu(Game* game) :
GameState_MenuState{game}
{
}

void GameState_CustomBattleLoadMenu::init(){

    boost::filesystem::directory_iterator end;

	if (boost::filesystem::exists(std::string("save"))){
		for (boost::filesystem::directory_iterator it(std::string("save")); it != end; ++it){
			menuList.push_back({it->path(), true, nullptr, Action::LOAD});
		}
	}

    menuList.push_back({std::string(""), false, nullptr, Action::NONE});
	menuList.push_back({std::string("Back"), true, game->CustomBattleMenuState.get(), Action::NONE});

	lineUpObjects();
}
