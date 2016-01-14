#pragma once

#include "xviii/GameStates/GameState.h"

#include "xviii/UI/SetupUI.h"
#include "xviii/Headers/global.h"
#include "xviii/Headers/keybinds.h"

class GameState_Setup : public GameState
{
public:
	GameState_Setup(Game* game);
	virtual void getInput();
	virtual void update(float mFT);
	virtual void draw();

	void oneTimeUpdate();

private:
	sf::Vector2f cameraVelocity;
	sf::Vector2i middleButtonCoords;
	bool middleButtonHeld{false};

	SetupUI setupUI;
	bool drawUI{true};

	std::unique_ptr<SpawnableUnit> selectedSpawnableUnit;

};
