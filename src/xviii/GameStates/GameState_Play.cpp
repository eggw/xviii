#include "xviii/Headers/stdafx.h"
#include "xviii/GameStates/GameState_Play.h"

#include "xviii/Core/Game.h"

std::string boolToString(bool _value){
	if (_value){
		return "YES";
	}
	else if (!_value){
		return "NO";
	}

	return "???";
}

GameState_Play::GameState_Play(Game* _game) :
GameState{_game},
selected{nullptr},
cameraVelocity{},
middleButtonCoords{},
middleButtonHeld{false},
tileDistanceText{},
playUI(game->mManager, this),
drawUI{true},
animationClock{}
{
	tileDistanceText.setFont(game->mManager.fontManager->getFont(FontManager::Type::TCMT));
	tileDistanceText.setString("");
	tileDistanceText.setCharacterSize(20);
}

void GameState_Play::oneTimeUpdate(){

    //elapsedTurns being set to 0 means that we are in the setup phase.
    if(game->mWorld->getElapsedTurns() == 0){
        game->mWorld->setElapsedTurns(1);
    }

    game->mWorld->unhighlightAllTiles();

    game->Player1->setDeploymentPoints(0);
    game->Player2->setDeploymentPoints(0);

	playUI.turnlyUpdate();

	for (auto& unit : game->inactivePlayer->getUnits()){
            unit->drawUnit = false;
            unit->drawFlag = false;
            unit->updateStats();
		}

		for (auto& unit : game->currentPlayer->getUnits()){
            unit->drawUnit = true;
            unit->drawFlag = true;
            game->mWorld->calculateViewDistance(unit.get(), true);
            unit->updateStats();
		}

    game->mWorld->highlightVisibleTiles();
}

void GameState_Play::getInput(){
	sf::Event event;

	while (game->mWindow.pollEvent(event)){
		switch (event.type){

		case sf::Event::Resized:
		    handleResize();
			break;

		case sf::Event::Closed:
			game->exitGame(false);
			break;

		case sf::Event::MouseMoved:
			game->mousePos.x = event.mouseMove.x;
			game->mousePos.y = event.mouseMove.y;
			break;

		case sf::Event::KeyPressed:

			switch (event.key.code){

			case Key::UP_KEY:
				cameraVelocity = {0.f, -2.f};
				break;

			case Key::RIGHT_KEY:
				cameraVelocity = {2.f, 0.f};
				break;

			case Key::DOWN_KEY:
				cameraVelocity = {0.f, 2.f};
				break;

			case Key::LEFT_KEY:
				cameraVelocity = {-2.f, 0.f};
				break;

			case Key::RESETZOOM_KEY:
				game->currentView->setSize(game->mWindow.getSize().x, game->mWindow.getSize().y);
				break;

			case Key::ZOOMIN_KEY:
				game->currentView->setSize(game->currentView->getSize().x - xResolution / 10, game->currentView->getSize().y - yResolution / 10);
				break;

			case Key::ZOOMOUT_KEY:
				game->currentView->setSize(game->currentView->getSize().x + xResolution / 10, game->currentView->getSize().y + yResolution / 10);
				break;

			case Key::NORTH_KEY:
				if (selected != nullptr){
					playUI.setCurrentMessageText(selected->rotate(UnitTile::Direction::N));
					playUI.setSaveStatus(false);
				}
				break;

			case Key::EAST_KEY:
				if (selected != nullptr){
					playUI.setCurrentMessageText(selected->rotate(UnitTile::Direction::E));
					playUI.setSaveStatus(false);
				}
				break;

			case Key::SOUTH_KEY:
				if (selected != nullptr){
					playUI.setCurrentMessageText(selected->rotate(UnitTile::Direction::S));
					playUI.setSaveStatus(false);
				}
				break;

			case Key::WEST_KEY:
				if (selected != nullptr){
					playUI.setCurrentMessageText(selected->rotate(UnitTile::Direction::W));
					playUI.setSaveStatus(false);
				}
				break;

			case Key::NEXT_TURN:
				game->currentPlayer->setReady(true);
				break;

			case Key::HIDE_UI_KEY:
				if (drawUI){
					drawUI = false;
				}
				else if (!drawUI){
					drawUI = true;
				}
				break;

			case Key::SHOW_GENERAL_RANGE_KEY:
                playUI.setDrawGeneralRangeIndicator(!playUI.getDrawGeneralRangeIndicator());
				break;

			case Key::LIMBER_KEY:
			{
				if (selected != nullptr){
					if (selected->hasLimberAbility()){
						playUI.setCurrentMessageText(selected->toggleLimber());
					}
				}
				break;
			}

			case Key::SQUARE_FORMATION_KEY:
			{
				if (selected != nullptr){
					if (selected->hasSquareFormationAbility()){
						playUI.setCurrentMessageText(selected->toggleSquareFormationActive());
					}
				}
				break;
			}

			case sf::Keyboard::Escape:
            {
                game->setGameState(game->CustomBattlePauseMenuState.get());
            }

			default: break;

			}

			break;

		case sf::Event::MouseWheelMoved:
			smoothZoom_zooming = true;
			smoothZoom_lerpFactor = 0.0085f;

			if (event.mouseWheel.delta > 0 && game->currentView->getSize().x > xResolution && game->currentView->getSize().y > yResolution){
                //zoom in
                //std::cout << game->currentView->getSize().x << ',' << game->currentView->getSize().y << std::endl;
				//game->currentView->setSize(game->currentView->getSize().x - xResolution / 12, game->currentView->getSize().y - yResolution / 12);
				smoothZoom_targetZoom = 0.8f;
			}
			else if (event.mouseWheel.delta < 0 && game->currentView->getSize().x < xResolution * 4 && game->currentView->getSize().y < yResolution * 4){
			    //zoom out
			    //std::cout << game->currentView->getSize().x << ',' << game->currentView->getSize().y << std::endl;
				//game->currentView->setSize(game->currentView->getSize().x + xResolution / 12, game->currentView->getSize().y + yResolution / 12);
				smoothZoom_targetZoom = 1.2f;
			}

			smoothZoom_currentZoom = 1.0f;
			smoothZoom_previousZoom = smoothZoom_currentZoom;

			smoothZoom_currentCenter = game->currentView->getCenter();

			if(smoothZoom_targetZoom >= 1.f){
				smoothZoom_targetCenter = game->mWindow.mapPixelToCoords(game->mousePos, *(game->currentView));
				auto dist = smoothZoom_targetCenter - smoothZoom_currentCenter;
				dist.x /= 2;
				dist.y /= 2;
				smoothZoom_targetCenter = smoothZoom_currentCenter - dist;
			}
			else{
				smoothZoom_targetCenter = game->mWindow.mapPixelToCoords(game->mousePos, *(game->currentView));
				auto dist = smoothZoom_targetCenter - smoothZoom_currentCenter;
				dist.x /= 2;
				dist.y /= 2;
				smoothZoom_targetCenter = smoothZoom_targetCenter - dist;
			}

			break;

		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Middle){
				middleButtonHeld = true;
				middleButtonCoords = {event.mouseButton.x, event.mouseButton.y};
			}

			else if (event.mouseButton.button == sf::Mouse::Left){
				sf::Vector2i mouseCoords{event.mouseButton.x, event.mouseButton.y};
				sf::Vector2i worldCoords{game->mWindow.mapPixelToCoords(mouseCoords, *game->currentView)};
				sf::Vector2f uiCoords{game->mWindow.mapPixelToCoords(game->mousePos, playUI.uiView)};

				if (uiCoords.x >= playUI.getButton().left() && uiCoords.x <= playUI.getButton().right()
					&&
					uiCoords.y >= playUI.getButton().top() && uiCoords.y <= playUI.getButton().bottom()
                    &&
                    drawUI){

					game->currentPlayer->setReady(true);

				}

				if (worldCoords.x >= game->mWorld->getDimensionsInPixels().x || worldCoords.y >= game->mWorld->getDimensionsInPixels().y ||
					worldCoords.x <= 0 || worldCoords.y <= 0){
					return;
				}

				auto unit = game->mWorld->unitAtPixelPos(worldCoords);
				auto terrain = game->mWorld->terrainAtPixelPos(worldCoords);
				bool friendly{true};

				//Cache the unit's allignment
				if (unit != nullptr){
					if (unit->getPlayer() != game->currentPlayer){
						friendly = false;
					}
				}


				if (selected != nullptr){

					//If the unit's movement "animation" is still playing,
					//teleport the unit to its destination instantly
                    if(moveAnimationPlaying){
                        selected->setSpritePixelPos(selected->getTerrain()->getPixelPos());
                        selected->updateStats();

                        moveAnimationPlaying = false;
					}

					//If a unit is selected and you click on a terrain tile,
					//move the unit there if it is unnocupied

					if (terrain != nullptr){
						bool occupied{false};

						if (terrain->getUnit() != nullptr){
                            occupied = true;
						}

						if (!occupied){
                            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
                                playUI.setCurrentMessageText(selected->attack(terrain));
                            }
                            else{
                                TerrainTile* initialTerrain = selected->getTerrain();
                                playUI.setCurrentMessageText(selected->moveTo(terrain));
                                selected->setSpritePixelPos(initialTerrain->getPixelPos());
                                moveAnimationPlaying = true;
                                selected->setHighlighted(false);
                            }

                            playUI.setSaveStatus(false);
						}

						//If the tile is occupied by an enemy unit, attack
						if (occupied){

                            if(!friendly){

                                if(unit->drawUnit || (!unit->drawUnit && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))){
                                    playUI.setCurrentMessageText(selected->attack(terrain));
                                    playUI.setSaveStatus(false);

                                    selected->modVector.clear();
                                    unit->modVector.clear();

                                    game->mWorld->clearDamagedUnits();

                                    const std::vector<UnitTile::unitPtr>& currentPlayerUnits = game->currentPlayer->getUnits();

									bool attackingUnitStillExists{false};

                                    for(auto& unit : currentPlayerUnits){
										if(unit.get() == selected){
											attackingUnitStillExists = true;
										}
                                    }

                                    if(!attackingUnitStillExists){
										selected = nullptr;
                                    }
                                }

                                else{
                                    TerrainTile* initialTerrain = selected->getTerrain();
                                    playUI.setCurrentMessageText(selected->moveTo(terrain));
                                    selected->setSpritePixelPos(initialTerrain->getPixelPos());
                                    moveAnimationPlaying = true;
                                    selected->setHighlighted(false);
                                }

                            }
                            else if(friendly){
                                playUI.setCurrentMessageText(selected->heal(unit));
                                playUI.setSaveStatus(false);
                                //specially for the general's heal ability
                            }

						}
					}
				}

				else if (selected == nullptr){
					//If you click a unit
					if (unit != nullptr && friendly){
						selected = unit;
						selected->setHighlighted(true);
					}
					else{
						middleButtonHeld = true;
						middleButtonCoords = {event.mouseButton.x, event.mouseButton.y};
					}

				}
			}

			else if (event.mouseButton.button == sf::Mouse::Right){

				if (selected != nullptr){

					//If the unit's movement "animation" is still playing,
					//teleport the unit to its destination instantly
                    if(moveAnimationPlaying){
                        selected->setSpritePixelPos(selected->getTerrain()->getPixelPos());

                        moveAnimationPlaying = false;
                        selected->updateStats();
					}

                    selected->setHighlighted(false);
					selected = nullptr;
				}

				playUI.clearCurrentMessageText();
			}

			break;

			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Middle){
					middleButtonHeld = false;
				}
				else if(event.mouseButton.button == sf::Mouse::Left){
					middleButtonHeld = false;
				}

				break;

			default: break;
		}
	}
}

void GameState_Play::handleWeather(){

    bool rain{false};
    float refreshTime{0};
    int intensity{0};

    for(auto& effect : game->mWorld->getWeatherEffects()){

        if(effect.first == World::Weather::HEAVY_RAIN){
            rain = true;
            refreshTime = 0.1;
            intensity = 400;
            break;
        }
        else if(effect.first == World::Weather::LIGHT_RAIN){
            rain = true;
            refreshTime = 0.3;
            intensity = 120;
            break;
        }

    }

    if(rain){
        if(animationClock.getElapsedTime().asSeconds() > refreshTime){

            game->mWorld->rainVector.clear();
            game->mWorld->rainVector.resize(intensity);

            boost::random::uniform_int_distribution<int> randomXCoordinate(0, game->mWorld->getDimensions().x);
            boost::random::uniform_int_distribution<int> randomYCoordinate(0, game->mWorld->getDimensions().y);

            for(int i{0}; i < intensity; ++i){
                game->mWorld->rainVector[i] = game->mWorld->masterManager.textureManager->getWeatherSprite("rain");
                game->mWorld->rainVector[i].setPosition(game->mWorld->pixelPosAtCartesianPos({{randomXCoordinate(game->mWorld->masterManager.randomEngine)},{randomYCoordinate(game->mWorld->masterManager.randomEngine)}}));
            }

            animationClock.restart();
        }
    }
}

void GameState_Play::update(float mFT){
	if (cameraVelocity.x >= -0.1 && cameraVelocity.x <= 0.1 && cameraVelocity.x != 0){
		cameraVelocity = {0, 0};
	}
	else if (cameraVelocity.x > 0){
		cameraVelocity.x -= 0.01f;
	}
	else if (cameraVelocity.x < 0){
		cameraVelocity.x += 0.01f;
	}

	if (cameraVelocity.y >= -0.5 && cameraVelocity.y <= 0.5 && cameraVelocity.y != 0){
		cameraVelocity = {0, 0};
	}
	else if (cameraVelocity.y > 0){
		cameraVelocity.y -= 0.01f;
	}
	else if (cameraVelocity.y < 0){
		cameraVelocity.y += 0.01f;
	}

	game->currentView->move(cameraVelocity * mFT);

	if (middleButtonHeld){
		sf::Vector2f resultantVector = (sf::Vector2f{middleButtonCoords} - sf::Vector2f{game->mousePos});
		resultantVector.x *= 0.0055;
		resultantVector.y *= 0.0055;
		game->currentView->move(resultantVector * mFT);
	}

	playUI.update();

    //////////////////////////////////////////////////////////////////////////////
	//MOVE ANIMATION LOGIC
	//////////////////////////////////////////////////////////////////////////////

	if(selected != nullptr){
        if(moveAnimationPlaying){
            sf::Vector2f currentPos = selected->getPixelPos();
            sf::Vector2f finalPos = selected->getTerrain()->getPixelPos();

            float xVariance = currentPos.x - finalPos.x;
            float yVariance = currentPos.y - finalPos.y;

            float modifier = 0;

            if(xVariance == 0 && yVariance == 0){
                moveAnimationPlaying = false;
                //During movement, highlights are disabled for aesthetic purposes.
                //Enable them again afterwards.
                selected->setHighlighted(true);
                selected->updateStats();
            }

            else if(xVariance == 0){

                if(yVariance < 0){
                    modifier = 0.5;
                }
                else{
                    modifier = -0.5;
                }

                selected->setSpritePixelPos({currentPos.x, currentPos.y + modifier});
            }

            else if(yVariance == 0){

                if(xVariance < 0){
                    modifier = 0.5;
                }
                else{
                    modifier = -0.5;
                }

                selected->setSpritePixelPos({currentPos.x + modifier, currentPos.y});
            }
        }
	}

    //////////////////////////////////////////////////////////////////////////////
	//TURNLY UPDATES
	//////////////////////////////////////////////////////////////////////////////

	if (game->currentPlayer->isReady()){

        game->mWorld->rainVector.clear();

        if (selected != nullptr){
            selected->setHighlighted(false);
            selected = nullptr;
        }

		game->nextPlayer();

        game->mWorld->turnlyUpdate();
        playUI.turnlyUpdate();

		for (auto& unit : game->inactivePlayer->getUnits()){
            unit->drawUnit = false;
            unit->drawFlag = false;
            unit->reset();
		}

		for (auto& unit : game->currentPlayer->getUnits()){
            unit->drawUnit = true;
            unit->drawFlag = true;
            game->mWorld->calculateViewDistance(unit.get(), true);
            unit->updateStats();
		}

		game->mWorld->highlightVisibleTiles();

	}


	//////////////////////////////////////////////////////////////////////////////
	//////////////WEATHER-RELATED UPDATES/////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////


    handleWeather();


	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	if (smoothZoom_zooming) {
		// Interpolate between the current zoom and the target zoom for a smooth effect.
		smoothZoom_currentZoom = lerp(smoothZoom_lerpFactor, smoothZoom_currentZoom, smoothZoom_targetZoom);
		game->currentView->zoom(1.0f + (smoothZoom_currentZoom - smoothZoom_previousZoom));

		// Interpolate the between the current center of the view and the target.
		smoothZoom_currentCenter.x = lerp(smoothZoom_lerpFactor, smoothZoom_currentCenter.x, smoothZoom_targetCenter.x);
		smoothZoom_currentCenter.y = lerp(smoothZoom_lerpFactor, smoothZoom_currentCenter.y, smoothZoom_targetCenter.y);
		game->currentView->setCenter(smoothZoom_currentCenter);

		// Store the previous zoom because we will need the difference between the previous and current zoom.
		// This is because sf::View::Zoom zooms relative to the current zoom.
		smoothZoom_previousZoom = smoothZoom_currentZoom;

		// If the difference between the current and previous zoom is less then 0.01, stop zooming.
		// Linear interpolation will never reach the target value so we stop here.
		if (fabs(smoothZoom_targetZoom - smoothZoom_currentZoom) < 0.01f)
			smoothZoom_zooming = false;
	}

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	//Code for the mouse indicator of distance:

	sf::Vector2i mouseLocation{game->mWindow.mapPixelToCoords(game->mousePos, *game->currentView)};
	sf::Vector2i worldDimensions{game->mWorld->getDimensionsInPixels()};

	if (mouseLocation.x < worldDimensions.x && mouseLocation.y < worldDimensions.y
		&&
		mouseLocation.x > 0 && mouseLocation.y > 0)
		{
			mousedOverTile = game->mWorld->terrainAtPixelPos(mouseLocation);
		}

	if (selected != nullptr && mousedOverTile != lastMousedOverTile){

		lastMousedOverTile = mousedOverTile;
		UnitTile* unit = mousedOverTile->getUnit();
		UnitTile::Direction dir = selected->getDir();

		bool validMovDirection{false};
		bool validAttackDirection{false};
		bool rangedObstructionPresent{false};
		bool meleeObstructionPresent{false};
		bool inMovementRange{false};
		bool inRangedAttackRange{false};

		//Store the distance between the selected tile and currently moused over tile in vectorDist
		sf::Vector2i vectorDist = selected->distanceFrom(mousedOverTile, validMovDirection, validAttackDirection, rangedObstructionPresent, meleeObstructionPresent, inMovementRange, inRangedAttackRange);

		int primaryAxisDistance{0};
		if (dir == UnitTile::Direction::N || dir == UnitTile::Direction::S){
			primaryAxisDistance = abs(vectorDist.y);
		}
		else{
			primaryAxisDistance = abs(vectorDist.x);
		}

		//For the direction string:
		std::string dir_string_X;
		std::string dir_string_Y;

		//N or S
		if (vectorDist.y < 0){
			dir_string_Y = "N";
		}
		else if (vectorDist.y > 0){
			dir_string_Y = "S";
		}

		//E or W
		if (vectorDist.x > 0){
			dir_string_X += "E";
		}
		else if (vectorDist.x < 0){
			dir_string_X += "W";
		}

		//If you aren't mousing over a (visible) enemy unit
		if ((unit == nullptr) || ((unit != nullptr && !unit->drawUnit) || !unit->drawFlag)){

			if (inMovementRange && !meleeObstructionPresent){
				tileDistanceText.setFillColor(sf::Color::Black);
				playUI.setArrowColor(sf::Color::Black);
			}
			else if (!inMovementRange && inRangedAttackRange){
				tileDistanceText.setFillColor(sf::Color::Magenta);
				playUI.setArrowColor(sf::Color::Magenta);
			}
			else{
				tileDistanceText.setFillColor(sf::Color::Red);
				playUI.setArrowColor(sf::Color::Red);
			}

		}
		//If you are mousing over a visible unit
		else if (unit != nullptr){

			//Check if it is an enemy unit
			bool friendly = (unit->getPlayer() == game->currentPlayer);

			if (((!validAttackDirection && (!selected->canHeal())) || meleeObstructionPresent || rangedObstructionPresent || (primaryAxisDistance > 1 && !inRangedAttackRange) || !selected->canMeleeAttack())){
					tileDistanceText.setFillColor(sf::Color::Red);
					playUI.setArrowColor(sf::Color::Red);
				}
				else{
					if (friendly && !selected->canHeal()){
						tileDistanceText.setFillColor(sf::Color::Magenta);
						playUI.setArrowColor(sf::Color::Magenta);
					}
					else{
						tileDistanceText.setFillColor(sf::Color::Blue);
						playUI.setArrowColor(sf::Color::Blue);
					}
				}

		}

		std::string finalString;

		bool yDistanceOverZero{abs(vectorDist.y) > 0};
		bool xDistanceOverZero{abs(vectorDist.x) > 0};

		if (yDistanceOverZero){
			finalString += std::to_string(abs(vectorDist.y)) + dir_string_Y;
		}
		if (xDistanceOverZero){
			if (yDistanceOverZero){
				finalString += " ";
			}
			finalString += std::to_string(abs(vectorDist.x)) + dir_string_X;
		}

		tileDistanceText.setString(finalString);

		mouseLocation.x -= 18;
		mouseLocation.y += 18;

		tileDistanceText.setPosition({float(mouseLocation.x), float(mouseLocation.y)});

	}
}

void GameState_Play::draw(){
	game->mWindow.setView(*game->currentView);
	game->mWorld->draw(game->mWindow);

	for (auto& player : game->mPlayers){
		player->drawUnits(game->mWindow);
	}

	if (selected != nullptr){
		game->mWindow.draw(tileDistanceText);
	}

	//
	if (drawUI){
		//playUI.draw() automatically changes views in order to draw what it needs to.
		//Keep this in mind, and always reset the view after calling it.
		playUI.draw(game->mWindow);
	}
	//
}

void GameState_Play::onSwitch(){
    handleResize();
}

void GameState_Play::handleResize(){
	game->Player1->view.setSize(game->mWindow.getSize().x, game->mWindow.getSize().y);
	game->Player2->view.setSize(game->mWindow.getSize().x, game->mWindow.getSize().y);

	sf::FloatRect viewport;
	viewport.width = playUI.barWidth / game->mWindow.getSize().x;
	viewport.height = playUI.barHeight / game->mWindow.getSize().y;
	viewport.left = (1 - viewport.width) / 2;
	viewport.top = 1 - viewport.height;
	playUI.uiView.setViewport(viewport);
}
