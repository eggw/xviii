#include "xviii/Headers/stdafx.h"
#include "xviii/UI/PlayUI.h"

#include "xviii/GameStates/GameState_Play.h"
#include "xviii/Core/Game.h"

sf::Clock PlayUI::generalRangeAnimationClock{};
bool PlayUI::generalRangeFadingOut{true};

sf::Clock PlayUI::outlineAnimationClock{};
bool PlayUI::outlineFadingOut{true};

PlayUI::PlayUI(MasterManager& _masterManager, GameState_Play* _gameState) :
UI(_masterManager),
gameState{_gameState},
elapsedTurnsText{},
messageLogText{},
currentMessageText{},
currentWeatherText{},
currentTimeText{},
saveText{},
squareFormationText{},
limberText{},
lancerBonusReadyText{},
generalRangeIndicator1{},
generalRangeIndicator2{},
arrow{sf::LineStrip, 2},
arrowTip1{sf::LineStrip, 2},
arrowTip2{sf::LineStrip, 2},
pseudoOutline{}
{
	button.text.setCharacterSize(20);
	button.setString("NEXT TURN");
	button.text.setOrigin(button.text.getLocalBounds().width / 2, button.text.getLocalBounds().height / 2);
	button.text.setPosition(button.sprite.getPosition().x, button.sprite.getPosition().y - 10);

	elapsedTurnsText.setFont(masterManager.fontManager->getFont(FontManager::Type::TCMT));
	elapsedTurnsText.setFillColor(sf::Color::Yellow);
	elapsedTurnsText.setPosition(20, 44);

	messageLogText.setFont(masterManager.fontManager->getFont(FontManager::Type::TCMT));
	messageLogText.setFillColor(sf::Color::White);
	messageLogText.setCharacterSize(15);
	messageLogText.setPosition(580, 4);
	messageLogText.setString("Messages");

	currentMessageText.setFont(masterManager.fontManager->getFont(FontManager::Type::TCMT));
	currentMessageText.setFillColor(sf::Color::Cyan);
	currentMessageText.setCharacterSize(19);
	currentMessageText.setPosition(340, 24);

	currentWeatherText.setFont(masterManager.fontManager->getFont(FontManager::Type::TCMT));
	currentWeatherText.setFillColor(sf::Color::White);
	currentWeatherText.setCharacterSize(30);
	currentWeatherText.setPosition(205, 4);

	currentTimeText.setFont(masterManager.fontManager->getFont(FontManager::Type::TCMT));
	currentTimeText.setFillColor(sf::Color::Cyan);
	currentTimeText.setCharacterSize(40);
	currentTimeText.setPosition(135, 94);


	saveText.setFont(masterManager.fontManager->getFont(FontManager::Type::TCMT));
	saveText.setCharacterSize(20);
	saveText.setPosition(20, 94);
	setSaveStatus(false);

	squareFormationText.setFont(masterManager.fontManager->getFont(FontManager::Type::TCMT));
	limberText.setFillColor(sf::Color::White);
	squareFormationText.setPosition(970, 4);

	limberText.setFont(masterManager.fontManager->getFont(FontManager::Type::TCMT));
	limberText.setFillColor(sf::Color::White);
	limberText.setPosition(970, 44);

	lancerBonusReadyText.setFont(masterManager.fontManager->getFont(FontManager::Type::TCMT));
	lancerBonusReadyText.setFillColor(sf::Color::White);
	lancerBonusReadyText.setPosition(970, 94);

	float rangeIndicatorThickness{5};


	generalRangeIndicator1.setFillColor(sf::Color::Transparent);
    generalRangeIndicator1.setOutlineThickness(rangeIndicatorThickness);
	generalRangeIndicator1.setOutlineColor(sf::Color(0,0,0));
    generalRangeIndicator1.setSize({float(masterManager.textureManager->getSize().x * 21), float(masterManager.textureManager->getSize().y * 21)});

    generalRangeIndicator1.setCornersRadius(20);
    generalRangeIndicator1.setCornerPointCount(20);

    generalRangeIndicator1.setOrigin((generalRangeIndicator1.getLocalBounds().width / 2) - rangeIndicatorThickness, (generalRangeIndicator1.getLocalBounds().height / 2) - rangeIndicatorThickness);

    generalRangeIndicator2.setFillColor(sf::Color::Transparent);
    generalRangeIndicator2.setOutlineThickness(rangeIndicatorThickness);
	generalRangeIndicator2.setOutlineColor(sf::Color(255,0,0));
    generalRangeIndicator2.setSize({float(masterManager.textureManager->getSize().x * 31), float(masterManager.textureManager->getSize().y * 31)});

    generalRangeIndicator2.setCornersRadius(20);
    generalRangeIndicator2.setCornerPointCount(20);

    generalRangeIndicator2.setOrigin((generalRangeIndicator2.getLocalBounds().width / 2) - rangeIndicatorThickness, (generalRangeIndicator2.getLocalBounds().height / 2) - rangeIndicatorThickness);

    float outlineThickness{1};

    pseudoOutline.setFillColor(sf::Color::Transparent);
    pseudoOutline.setOutlineThickness(outlineThickness);
	pseudoOutline.setOutlineColor(sf::Color::Yellow);
    pseudoOutline.setSize({float(masterManager.textureManager->getSize().x), float(masterManager.textureManager->getSize().y)});

    pseudoOutline.setOrigin((pseudoOutline.getLocalBounds().width / 2) - outlineThickness, (pseudoOutline.getLocalBounds().height / 2) - outlineThickness);

}

PlayUI::~PlayUI(){
}

void PlayUI::setElapsedTurnsText(int _num){
	elapsedTurnsText.setString("Turn " + std::to_string(_num));
}

void PlayUI::setCurrentWeatherText(){
    auto weatherEffects = gameState->game->mWorld->getWeatherEffects();

    std::string tempStr{};

    if(weatherEffects.empty()){
        tempStr = "clear";
    }

    for(auto& effect : weatherEffects){
        #define X(_str, _weather, _u, _f)\
            if(effect.first == _weather){\
                tempStr += _str;\
                tempStr += "\n";\
            }
            WEATHERPROPERTIES
            #undef X
    }


    currentWeatherText.setString(tempStr);
}

void PlayUI::setCurrentTimeText(std::pair<int,int> _time){
    std::string minutesStr;

    if(_time.second < 10){
        minutesStr = "0" + std::to_string(_time.second);
    }
    else{
        minutesStr = std::to_string(_time.second);
    }
	currentTimeText.setString(std::to_string(_time.first) + ":" + minutesStr);

	if(gameState->game->mWorld->getIsNighttime()){
        currentTimeText.setFillColor(sf::Color::Cyan);
	}
	else{
        currentTimeText.setFillColor(sf::Color::Yellow);
	}

}

void PlayUI::setCurrentMessageText(std::string _str){
	currentMessageText.setString(_str);
}

void PlayUI::clearCurrentMessageText(){
	currentMessageText.setString("");
}

void PlayUI::setSaveStatus(bool val){
	if (val){
		saveText.setString("Saved!");
	}
	else{
		saveText.setString("Not saved");
	}
}

void PlayUI::update(){
	if (gameState->game->currentPlayer->isReady()){
		clearCurrentMessageText();
		setSaveStatus(false);
	}

	drawLimberText = false;
	drawSquareFormationText = false;
	drawLancerBonusReadyText = false;

	///////////////////General range indicators//////////////////////////
	if((gameState->selected == gameState->game->currentPlayer->getGeneral() || drawGeneralRangeIndicator)
		&&
		gameState->game->currentPlayer->getGeneral() != nullptr){
            UnitTile* general = gameState->game->currentPlayer->getGeneral();
            generalRangeIndicator1.setPosition(general->getTerrain()->getPixelPosCenter());
            generalRangeIndicator2.setPosition(general->getTerrain()->getPixelPosCenter());


            auto generalTransparency = generalRangeIndicator1.getOutlineColor().a;

            int generalModifier;
            if(generalRangeFadingOut){
                generalModifier = -1;
            }
            else{
                generalModifier = 1;
            }

            if(generalRangeAnimationClock.getElapsedTime().asMicroseconds() > 200){
                generalTransparency += generalModifier;
                generalRangeAnimationClock.restart();
            }

            if(generalTransparency <= 50){
                generalRangeFadingOut = false;
            }
            else if(generalTransparency == 255){
                generalRangeFadingOut = true;
            }

            generalRangeIndicator1.setOutlineColor(sf::Color(0,0,0,generalTransparency));
            generalRangeIndicator2.setOutlineColor(sf::Color(255,0,0,generalTransparency));
		}
	/////////////////////////////////////////////////////////////////////

	if (gameState->selected != nullptr){

		if (gameState->selected->hasLimberAbility()){
			drawLimberText = true;

			if (gameState->selected->getLimber()){
				limberText.setString("Limber ON");
			}
			else{
				limberText.setString("Limber OFF");
			}
		}

		if (gameState->selected->hasSquareFormationAbility()){
			drawSquareFormationText = true;

			if (gameState->selected->getSquareFormationActive()){
				squareFormationText.setString("S.F. ON");
			}
			else{
				squareFormationText.setString("S.F. OFF");
			}
		}

		if (gameState->selected->hasLancerAbility()){
			drawLancerBonusReadyText = true;

			if (gameState->selected->getLancerBonusReady()){
				lancerBonusReadyText.setString("Lncr bns RDY");
			}
			else{
				lancerBonusReadyText.setString("Lncr bns NT RDY");
			}
		}

		/////////////////////////////////////////////////////////////
		//Management of the arrow and pseudo outline
		/////////////////////////////////////////////////////////////

		bool arrowVisible{true};

		//The code here is to make sure that the beginning and edges of the arrow are properly aligned

		sf::Vector2i mouseLocation{gameState->game->mWindow.mapPixelToCoords(gameState->game->mousePos, *(gameState->game->currentView))};

        TerrainTile* terrain = gameState->game->mWorld->terrainAtPixelPos(mouseLocation);

        if(terrain != nullptr){

            pseudoOutline.setPosition(terrain->getPixelPosCenter());

            sf::Vector2i terrainCartesianPos{terrain->getCartesianPos()};
            sf::Vector2i currentCartesianPos{gameState->selected->getTerrain()->getCartesianPos()};
            sf::Vector2i distance{currentCartesianPos - terrainCartesianPos};

            sf::Vector2f finalPosition1{};
            sf::Vector2f finalPosition2{};

            //The first if statements looks a little loaded.
            //It prevents the arrow from being drawn if the selected tile is equivalent to the
            //currently selected unit, or directly adjacent to it, by setting both positions to
            //the center of the current tile. Therefore, the arrow won't be visible. By setting
            //the variable arrowVisible, we determine whether the arrowheads's calculation should
            //result in them being visible or not.

            //The other statements determine the exact starting and ending positions of the arrow.

            int offset{25};

            if(((abs(distance.x) + abs(distance.y)) <= 2) && abs(distance.x) < 2 && abs(distance.y) < 2){
                finalPosition1 = gameState->selected->getTerrain()->getPixelPosCenter();
                finalPosition2 = gameState->selected->getTerrain()->getPixelPosCenter();
                arrowVisible = false;
            }
            else if(abs(distance.x) >= abs(distance.y)){
                if(distance.x < 0){
                    finalPosition1 = {float(gameState->selected->right() + offset), float(gameState->selected->getTerrain()->getPixelPosCenter().y)};
                    finalPosition2 = {float(terrain->left() - 15), float(terrain->getPixelPosCenter().y)};
                }
                else{
                    finalPosition1 = {float(gameState->selected->left() - offset), float(gameState->selected->getTerrain()->getPixelPosCenter().y)};
                    finalPosition2 = {float(terrain->right() + 15), float(terrain->getPixelPosCenter().y)};
                }
            }
            else if((abs(distance.x) < abs(distance.y))){
                if(distance.y < 0){
                    finalPosition1 = {float(gameState->selected->getPixelPosCenter().x), float(gameState->selected->getTerrain()->bottom() + offset)};
                    finalPosition2 = {float(terrain->getPixelPosCenter().x), float(terrain->top() - 15)};
                }
                else{
                    finalPosition1 = {float(gameState->selected->getPixelPosCenter().x), float(gameState->selected->getTerrain()->top() - offset)};
                    finalPosition2 = {float(terrain->getPixelPosCenter().x), float(terrain->bottom() + 15)};
                }
            }

            arrow[0].position = finalPosition1;
            arrow[1].position = finalPosition2;
		}

		///////////////////////////////////////////////////////////////////////////////
		//Now, with a little vector math, we do some calculations to determine how the
		//arrowheads should be placed.
		///////////////////////////////////////////////////////////////////////////////
        if(arrowVisible){
            arrowTip1[0].position = arrow[1].position;
            arrowTip2[0].position = arrow[1].position;

            float h = 10*sqrt(3);
            float w = 10;

            sf::Vector2f minusVector = {arrow[1].position - arrow[0].position};
            float length{(minusVector.x * minusVector.x) + (minusVector.y * minusVector.y)};
            length = sqrt(length);

            sf::Vector2f U = minusVector/length;
            sf::Vector2f V{-U.y, U.x};

            arrowTip1[1].position = arrow[1].position - h*U + w*V;
            arrowTip2[1].position = arrow[1].position - h*U - w*V;
        }
        else{
            arrowTip1[0].position = arrow[0].position;
            arrowTip2[0].position = arrow[0].position;

            arrowTip1[1].position = arrow[0].position;
            arrowTip2[1].position = arrow[0].position;
        }

        ///////////////////////////////////////////////////////////////////////////////

		//Once the positions are properly set, we manage the outline fading

        auto outlineTransparency = pseudoOutline.getOutlineColor().a;

        int outlineModifier;
        if(outlineFadingOut){
            outlineModifier = -1;
        }
        else{
            outlineModifier = 1;
        }

        if(outlineAnimationClock.getElapsedTime().asMicroseconds() > 20){
            outlineTransparency += outlineModifier;
            outlineAnimationClock.restart();
        }

        if(outlineTransparency <= 50){
            outlineFadingOut = false;
        }
        else if(outlineTransparency == 255){
            outlineFadingOut = true;
        }



        arrow[0].color.a = outlineTransparency;
        arrow[1].color.a = outlineTransparency;
        arrowTip1[0].color.a  = outlineTransparency;
        arrowTip1[1].color.a = outlineTransparency;
        arrowTip2[0].color.a  = outlineTransparency;
        arrowTip2[1].color.a = outlineTransparency;

        pseudoOutline.setOutlineColor(sf::Color(255,255,0,outlineTransparency));

        /////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////

	}

	//For the highlighting of the next turn button:
	sf::Vector2f uiCoords{gameState->game->mWindow.mapPixelToCoords(gameState->game->mousePos, uiView)};

	if (uiCoords.x >= button.left() && uiCoords.x <= button.right()
		&&
		uiCoords.y >= button.top() && uiCoords.y <= button.bottom()){

		setButtonHighlighted(true);
	}
	else{
		setButtonHighlighted(false);
	}
}

void PlayUI::turnlyUpdate(){
    setCurrentPlayerText(gameState->game->currentPlayer->getShortDisplayName());
    setElapsedTurnsText(gameState->game->mWorld->getElapsedTurns());
    setCurrentTimeText(gameState->game->mWorld->getCurrentTime().getTime());
    setCurrentWeatherText();
}

void PlayUI::draw(sf::RenderTarget &target, sf::RenderStates /*states*/) const{
    //Normally, the world view should be already set when this function is called
    if(gameState->selected != nullptr){
        if(gameState->selected == gameState->game->currentPlayer->getGeneral()){
            target.draw(generalRangeIndicator2);
            target.draw(generalRangeIndicator1);
        }

        target.draw(pseudoOutline);
        target.draw(arrow);
        target.draw(arrowTip1);
        target.draw(arrowTip2);
    }
    if(drawGeneralRangeIndicator && gameState->game->currentPlayer->getGeneral() != nullptr){
		target.draw(generalRangeIndicator2);
		target.draw(generalRangeIndicator1);
    }

	//After drawing the general range indicators and the arrow, we move on to the UI view
	gameState->game->mWindow.setView(uiView);

	target.draw(uiSprite);
	target.draw(currentPlayerText);
	target.draw(currentWeatherText);
	target.draw(currentTimeText);
	target.draw(elapsedTurnsText);
	target.draw(saveText);
    target.draw(currentMessageText);

	if (drawLimberText){
		target.draw(limberText);
	}
	if (drawSquareFormationText){
		target.draw(squareFormationText);
	}
	if (drawLancerBonusReadyText){
		target.draw(lancerBonusReadyText);
	}

	target.draw(messageLogText);

	//ready button
	target.draw(button.sprite);
	target.draw(button.text);

	if (button.highlighted){
		target.draw(button.rekt);
	}
	//
}
