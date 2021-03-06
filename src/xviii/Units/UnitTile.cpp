#include "xviii/Headers/stdafx.h"
#include "xviii/Units/UnitTile.h"

#include "xviii/Core/Player.h"
#include "xviii/Core/World.h"

#include "xviii/Core/UnitLoader.h"

void UnitTile::setTerrain(TerrainTile* _terrain){
    if(terrain != nullptr){
        terrain->resetUnit();
    }

    terrain = _terrain;
    terrain->setUnit(this);
    truePosition = terrain->getCartesianPos();
}

bool UnitTile::getMelee() const{ return world->masterManager.unitLoader->customClasses.at(unitID).melee; }
bool UnitTile::getSkirmish() const{ return world->masterManager.unitLoader->customClasses.at(unitID).skirmish; }
bool UnitTile::getFrightening() const { return world->masterManager.unitLoader->customClasses.at(unitID).frightening; }
bool UnitTile::getHalfRangedDamage() const { return world->masterManager.unitLoader->customClasses.at(unitID).halfRangedDamage; }
bool UnitTile::canHeal() const{ return !(world->masterManager.unitLoader->customClasses.at(unitID).healingRangeValues.empty()); }
bool UnitTile::hasRangedCapability() const{ return !(world->masterManager.unitLoader->customClasses.at(unitID).rangedAttackDistValues.empty()); }
bool UnitTile::canAttackBridge() const{ return !(world->masterManager.unitLoader->customClasses.at(unitID).bridgeAttackDistValues.empty()); }
bool UnitTile::getCanShootOverUnits() const { return world->masterManager.unitLoader->customClasses.at(unitID).canShootOverUnits; }

int UnitTile::getChargesPerTurn() const { return world->masterManager.unitLoader->customClasses.at(unitID).chargesPerTurn; }
int UnitTile::getShotsPerTurn() const { return world->masterManager.unitLoader->customClasses.at(unitID).shotsPerTurn; }

bool UnitTile::hasSquareFormationAbility() const{ return world->masterManager.unitLoader->customClasses.at(unitID).hasSquareFormationAbility; }
bool UnitTile::hasLimberAbility() const{ return world->masterManager.unitLoader->customClasses.at(unitID).hasLimberAbility; }
bool UnitTile::hasLancerAbility() const{ return world->masterManager.unitLoader->customClasses.at(unitID).hasLancerAbility; }

int UnitTile::getCost() const{ return world->masterManager.unitLoader->customClasses.at(unitID).cost; }
int UnitTile::getLimit() const{ return world->masterManager.unitLoader->customClasses.at(unitID).limit; }
int UnitTile::getMaxHp() const{ return world->masterManager.unitLoader->customClasses.at(unitID).maxHp; }
int UnitTile::getMaxMov() const{ return world->masterManager.unitLoader->customClasses.at(unitID).maxMov; }
int UnitTile::getConeWidth() const{ return world->masterManager.unitLoader->customClasses.at(unitID).coneWidth; }

int UnitTile::getDefaultUnitViewDistance() const {return world->masterManager.unitLoader->customClasses.at(unitID).unitViewDistance;}
int UnitTile::getDefaultFlagViewDistance() const {return world->masterManager.unitLoader->customClasses.at(unitID).flagViewDistance;}

void UnitTile::applyBonusModifiers(UnitTile* _unit, bool _attacking){
	UnitTile::UnitType mainType = _unit->getUnitType();
	UnitTile::UnitFamily familyType = _unit->getUnitFamilyType();
	std::string enemyUnitID = _unit->getUnitID();

	auto& bonusesVsMainTypes = world->masterManager.unitLoader->customClasses.at(unitID).bonusesVsMainTypes;
	auto& bonusesVsFamilyTypes = world->masterManager.unitLoader->customClasses.at(unitID).bonusesVsFamilyTypes;
	auto& bonusesVsUnitIDs = world->masterManager.unitLoader->customClasses.at(unitID).bonusesVsUnitIDs;

	/////////////////////////////////////////////////////////////////////////////////////////

	for (auto& bonus : bonusesVsMainTypes){
		if (bonus.mainType == mainType){
			if ((!_attacking && bonus.whenDefending) || (_attacking && bonus.whenAttacking)){
				modVector.emplace_back(Modifier::BONUS, bonus.modifier, bonus.modifierIsAdditional);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////

	for (auto& bonus : bonusesVsFamilyTypes){
		if (bonus.familyType == familyType){
			if ((!_attacking && bonus.whenDefending) || (_attacking && bonus.whenAttacking)){
				modVector.emplace_back(Modifier::BONUS, bonus.modifier, bonus.modifierIsAdditional);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////

	for (auto& bonus : bonusesVsUnitIDs){
		if (bonus.unitID == enemyUnitID){
			if ((!_attacking && bonus.whenDefending) || (_attacking && bonus.whenAttacking)){
				modVector.emplace_back(Modifier::BONUS, bonus.modifier, bonus.modifierIsAdditional);
			}
		}
	}
}

void UnitTile::applyFlankModifier(Modifier _flank, UnitTile* _enemy){
	if (_enemy->hasSquareFormationAbility() && _enemy->getSquareFormationActive()){
		this->modVector.emplace_back(Modifier::SQUARE_FORMATION, world->masterManager.unitLoader->customClasses.at(unitID).squareFormationModifier, false);
	}

	else{
		this->modVector.emplace_back(_flank, getFlankModifier(_enemy->getUnitType(), _flank), false);
	}
}

void UnitTile::applyTerrainModifiers(TerrainTile* _terrain, int _distance, bool _attacking){
	_terrain->applyModifiers(this, _distance, _attacking);
}

float UnitTile::getFlankModifier(UnitType _mainType, Modifier _flank) const{
	auto& vec = world->masterManager.unitLoader->customClasses.at(unitID).flankModifierValues;

	for (auto& flankModifier : vec){
		if (flankModifier.type == _mainType){
			switch (_flank){
			case Modifier::FRONT_FLANK:
				return flankModifier.front;
				break;
			case Modifier::SIDE_FLANK:
				return flankModifier.side;
				break;
			case Modifier::REAR_FLANK:
				return flankModifier.rear;
				break;
            default:
                break;
			}
		}
	}

	return 0;
}

UnitTile::Direction UnitTile::opposite(UnitTile::Direction _dir){
	switch (_dir){

	case UnitTile::Direction::N:
		return UnitTile::Direction::S;

	case UnitTile::Direction::S:
		return UnitTile::Direction::N;

	case UnitTile::Direction::E:
		return UnitTile::Direction::W;

	case UnitTile::Direction::W:
		return UnitTile::Direction::E;
	}

	return UnitTile::Direction::N;
}

//virtual
int UnitTile::getMaxRange() const{
	if (!hasRangedCapability()){
		return 0;
	}
	else{
		//Of course, assuming the elements of rangedAttackDistValues were inserted in order of
		//furthest to shortest distances, the first element's upper threshold should represent
		//the furthest a unit can possibly shoot
		return world->masterManager.unitLoader->customClasses.at(unitID).rangedAttackDistValues[0].upperThreshold;
	}
}

std::string UnitTile::roundFloat(const double x) {
	std::stringstream ss;
	ss << std::fixed;

	//This portion checks if it is "whole" (.0) or "half" (.5)
	//and sets precision accordingly, so that if the number is just
	//5 it will display as 5 rather than the redundant 5.0 and if
	//it is 5.5 it will display as 5.5

	int test = static_cast<int>(x * 10) % 10;

	if (test == 5){
		ss.precision(1);
	}
	else{
		ss.precision(0);
	}

	//

	ss << x;
	return ss.str();
}


std::string UnitTile::dirToString(){
    switch(dir){
    case UnitTile::Direction::N:
        return "N";

    case UnitTile::Direction::E:
        return "E";

    case UnitTile::Direction::S:
        return "S";

    case UnitTile::Direction::W:
        return "W";
    }

    return{"???"};
}

std::string UnitTile::modToString(ModifierReport _mod){
	//All modifiers are multiplicational, except (obviously) ADDITIONAL
	switch (_mod.modType){
	case Modifier::NONE:
		return{"none"};

    case Modifier::ATK:
        return{"atk"};

    case Modifier::DFND:
        return{"dfnd"};

	case Modifier::TERRAIN:
		return{"terrain"};

	case Modifier::BONUS:
		if (_mod.modFloat >= 0){
			return{"bonus"};
		}
		else{
			return{"malus"};
		}

	case Modifier::DISTANCE:
		return{"distance"};

	case Modifier::FRONT_FLANK:
		return{"front flank"};

	case Modifier::SIDE_FLANK:
		return{"side flank"};

	case Modifier::REAR_FLANK:
		return{"rear flank"};

	case Modifier::SQUARE_FORMATION:
		return{"square formation"};
	}

	return{"???"};
}

UnitTile::UnitTile(World* _world, Player* _belongsToPlayer, std::string _unitID, UnitTile::UnitType type, UnitTile::UnitFamily familyType, Direction _dir) :
Tile(_world, _world->masterManager.unitLoader->customClasses.at(_unitID).textureID),
player{_belongsToPlayer},
dir{_dir},
terrain{nullptr},
unitID{_unitID},
unitType{type},
unitFamilyType{familyType},
hp{0},
mov{0},
coneWidth{1},
canShootOverUnits{false}
{
    currentUnitViewDistance = getDefaultUnitViewDistance();
    currentFlagViewDistance = getDefaultFlagViewDistance();

    displayName = world->masterManager.unitLoader->customClasses.at(unitID).displayName;
    shortDisplayName = world->masterManager.unitLoader->customClasses.at(unitID).shortDisplayName;

	unitFlag = player->getNationFlag();
	unitFlag.setColor(sf::Color(255,255,255, 100));

    dirText.setString(dirToString());

    dirText.setFont(world->masterManager.fontManager->getFont(FontManager::Type::TCMT));
    hpText.setFont(world->masterManager.fontManager->getFont(FontManager::Type::TCMT));
    movText.setFont(world->masterManager.fontManager->getFont(FontManager::Type::TCMT));

    dirText.setCharacterSize(17);
    hpText.setCharacterSize(17);
    movText.setCharacterSize(17);

    dirText.setFillColor(sf::Color(0,0,180));
    hpText.setFillColor(sf::Color::Red);
    movText.setFillColor(sf::Color::Black);

    generalRange1 = world->masterManager.textureManager->getSprite(TextureManager::UI::GENERALRANGE1);
    //Opacity to be finalized
    //generalRange1.setColor(sf::Color(255,255,255, 190));
    generalRange2 = world->masterManager.textureManager->getSprite(TextureManager::UI::GENERALRANGE2);
    //generalRange2.setColor(sf::Color(255,255,255, 190));
    generalDead = world->masterManager.textureManager->getSprite(TextureManager::UI::GENERALDEAD);
    //generalDead.setColor(sf::Color(255,255,255, 190));

	outline.setPosition(sprite.getPosition().x, sprite.getPosition().y);
	outline.setSize(sf::Vector2f(world->masterManager.textureManager->getSize().x, world->masterManager.textureManager->getSize().y));
	outline.setOutlineColor(sf::Color::Yellow);
	outline.setOutlineThickness(-1);
	outline.setFillColor(sf::Color::Transparent);

	hp = getMaxHp();
	mov = getMaxMov();
	coneWidth = getConeWidth();
	canShootOverUnits = getCanShootOverUnits();
}

void UnitTile::spawn(TerrainTile* terrainTile){
	if (terrain != nullptr){
		terrain->resetUnit();
	}

	setTerrain(terrainTile);
	updateStats();
}

//Virtual
std::string UnitTile::moveTo(TerrainTile* _terrainTile){
	if (getSquareFormationActive()){
		return SF_ACTIVE;
	}

	bool validAttackDirection{false};
	bool rangedObstructionPresent{false};
	bool inRangedAttackRange{false};

	bool validMovDirection{false};
	bool meleeObstructionPresent{false};
	bool inMovementRange{false};
	int movExpended{0};

	//Get the coordinates of the current tile the unit is at
	sf::Vector2i currentCoords{world->cartesianPosAtIndex(world->indexAtTile(*terrain))};

	//Get the coordinates of the tile to be moved to
	sf::Vector2i toMoveToCoords{world->cartesianPosAtIndex(world->indexAtTile(*_terrainTile))};

	distanceFrom(_terrainTile, validMovDirection, validAttackDirection, rangedObstructionPresent, meleeObstructionPresent, inMovementRange, inRangedAttackRange);

	if (meleeObstructionPresent){
		return OBSTRUCTION_PRESENT_MOV;
	}

	else if (validMovDirection && inMovementRange){

        int PRIMARYAXIS_CURRENT;
        int PRIMARYAXIS_DESTINATION;
        int SECONDARYAXIS_CURRENT;
        //Declared but unused; commented out in case I'll need it in the future
        //int SECONDARYAXIS_DESTINATION;

        int PRIMARYAXIS_MOVEMENT;

        switch (dir){
            case UnitTile::Direction::N:
                PRIMARYAXIS_CURRENT = currentCoords.y;
                PRIMARYAXIS_DESTINATION = toMoveToCoords.y;
                SECONDARYAXIS_CURRENT = currentCoords.x;
                //SECONDARYAXIS_DESTINATION = toMoveToCoords.x;

                PRIMARYAXIS_MOVEMENT = -1;
                break;

            case UnitTile::Direction::E:
                PRIMARYAXIS_CURRENT = currentCoords.x;
                PRIMARYAXIS_DESTINATION = toMoveToCoords.x;
                SECONDARYAXIS_CURRENT = currentCoords.y;
                //SECONDARYAXIS_DESTINATION = toMoveToCoords.y;

                PRIMARYAXIS_MOVEMENT = 1;
                break;

            case UnitTile::Direction::S:
                PRIMARYAXIS_CURRENT = currentCoords.y;
                PRIMARYAXIS_DESTINATION = toMoveToCoords.y;
                SECONDARYAXIS_CURRENT = currentCoords.x;
                //SECONDARYAXIS_DESTINATION = toMoveToCoords.x;

                PRIMARYAXIS_MOVEMENT = 1;
                break;


            case UnitTile::Direction::W:
                PRIMARYAXIS_CURRENT = currentCoords.x;
                PRIMARYAXIS_DESTINATION = toMoveToCoords.x;
                SECONDARYAXIS_CURRENT = currentCoords.y;
                //SECONDARYAXIS_DESTINATION = toMoveToCoords.y;
                PRIMARYAXIS_MOVEMENT = -1;

                break;
        }

        sf::Vector2i finalCoords{toMoveToCoords};

        bool exitLoop{false};

        //sf::Clock animationTimer;
        //Begin at the tile that comes after the current tile, and loop to the destination (inclusive)
        for (int i{PRIMARYAXIS_CURRENT + PRIMARYAXIS_MOVEMENT}; i != PRIMARYAXIS_DESTINATION && !exitLoop; i += PRIMARYAXIS_MOVEMENT){

            TerrainTile* terrainInTheWay;
            int indexToCheck = i;

            if (dir == Direction::N || dir == Direction::S){
                terrainInTheWay = world->terrainAtCartesianPos({SECONDARYAXIS_CURRENT, indexToCheck});
            }
            else{
                terrainInTheWay = world->terrainAtCartesianPos({indexToCheck, SECONDARYAXIS_CURRENT});
            }

            if(world->calculateViewDistance(this, terrainInTheWay, false)){

                if (dir == Direction::N || dir == Direction::S){
                    finalCoords.x = SECONDARYAXIS_CURRENT;
                    finalCoords.y = i;
                }

                else{
                    finalCoords.x = i;
                    finalCoords.y = SECONDARYAXIS_CURRENT;
                }

                exitLoop = true;
            }

            //while(animationTimer.getElapsedTime().asSeconds() < 0.5f){

            //}

            //playMoveToAnimation(terrainInTheWay);
        }


        if (dir == Direction::N || dir == Direction::S){
            movExpended = abs(finalCoords.y - currentCoords.y);
        }
        else{
            movExpended = abs(finalCoords.x - currentCoords.x);
        }

        TerrainTile* destination = world->terrainAtCartesianPos(finalCoords);

        //If we call wearDownBridges now, and the terrain tile we were standing on was
        //a bridge with 1 hp left, it will be deleted and terrain will be a dangling
        //pointer.

        TerrainTile* oldTerrain = terrain;

		setTerrain(destination);
		mov -= movExpended;
		hasMoved = true;

		//If we only move one tile, the view distance won't be recalculated in the loop,
		//so just do it again here anyway

        world->calculateViewDistance(this, false);
        world->highlightVisibleTiles();

		updateStats();

		world->wearDownTempBridges(oldTerrain, destination);

		if(finalCoords == toMoveToCoords){
            return MOV_SUCCESS + std::to_string(finalCoords.x + 1) + ", " + std::to_string(finalCoords.y + 1);
        }

        else{
            return "Movement disrupted!";
        }
	}

	else if (validMovDirection && !inMovementRange){
		return{NO_MOV};
	}

	else if (!validMovDirection){
		return{INVALID_DIR_MOV};
	}

    return{"???"};
}

void UnitTile::playMoveToAnimation(TerrainTile* _terrain){
    sf::Vector2f destinationPost = _terrain->getPixelPos();
    this->sprite.setPosition(destinationPost);
    updateStats();
}

//Virtual
void UnitTile::reset(){
	calculateEffectiveMov();

	if (!limber && hasLimberAbility()){
		mov = 0;
	}

	if (hasLancerAbility()){
		if (!getHasMeleeAttacked()){
			lancerBonusReady = true;
		}
		else{
			lancerBonusReady = false;
		}
	}

	hasHealed = false;
	hasMoved = false;
	hasPartialRotated = false;
	hasFullRotated = false;
	meleeAttacks = 0;
	rangedAttacks = 0;
	updateStats();
}

void UnitTile::setSpritePixelPos(sf::Vector2f _pos){
    sprite.setPosition(_pos);
    unitFlag.setPosition(_pos);
    outline.setPosition(_pos);

	generalRange1.setPosition(getPixelPosCenter().x, bottom() - generalRange1.getTexture()->getSize().y);
	generalRange2.setPosition(getPixelPosCenter().x, bottom() - generalRange2.getTexture()->getSize().y);
	generalDead.setPosition(getPixelPosCenter().x, bottom() - generalDead.getTexture()->getSize().y);

    //////////////////////////////////////////////

    //Update the physical position of the stats, and sprite, if needed
    //This algorithm attempts to place the numbers in as even a space as possible:
    auto spriteSize = sprite.getLocalBounds();
	auto spritePos = sprite.getPosition();

    int combinedWidth = dirText.getLocalBounds().width + hpText.getLocalBounds().width + movText.getLocalBounds().width;
    int edgeSize = (spriteSize.width - combinedWidth) / 4;

    dirText.setPosition(spritePos.x + edgeSize, spritePos.y + spriteSize.height);
    movText.setPosition(spritePos.x + spriteSize.width - edgeSize - movText.getLocalBounds().width, spritePos.y + spriteSize.height);
    hpText.setPosition((dirText.getPosition().x + dirText.getLocalBounds().width + movText.getPosition().x) / 2 - hpText.getLocalBounds().width / 2 , spritePos.y + spriteSize.height);
}

void UnitTile::stun(){
	mov = 0;
	meleeAttacks = getChargesPerTurn();
	rangedAttacks = getShotsPerTurn();
	hasMoved = true;
}

std::string UnitTile::toggleSquareFormationActive(){
	if (squareFormationActive){
		squareFormationActive = false;
		stun();
	}
	else{
		if (getHasAnyAttacked()){
			return CANNOT_DO_THAT_ANYMORE;
		}
		else{
			squareFormationActive = true;
		}
	}

	updateStats();
	return{};
}


std::string UnitTile::toggleLimber(){
	if (limber){
		limber = false;
		stun();
	}
	else{
		if (getHasAnyAttacked()){
			return CANNOT_DO_THAT_ANYMORE;
		}
		else{
			limber = true;
		}
	}

	updateStats();
	return{};
}

std::string UnitTile::heal(UnitTile* _unit){
	if (canHeal()){
		sf::Vector2i thisPos = getCartesianPos();
		sf::Vector2i friendlyPos = _unit->getCartesianPos();

		sf::Vector2i difference = (thisPos - friendlyPos);
		difference.x = abs(difference.x);
		difference.y = abs(difference.y);

		float healingAmount{0};

		//(healingRange.lowerThreshold == 0 && healingRange.upperThreshold == 0)
		//Infinite range if the thresholds are both 0

		// || difference.axis == 0
		//This is to ensure that even when one of the axes is 0, the unit will still get properly healed, if in range
		//(Example: with a range of 1-6, if you are on the same x-axis, you may have a distance of 0, 5. It should still
		//be considered valid.)

		for (auto & healingRange : world->masterManager.unitLoader->customClasses.at(unitID).healingRangeValues){
			if ((healingRange.lowerThreshold == 0 && healingRange.upperThreshold == 0)
				||
				((difference.x >= healingRange.lowerThreshold || difference.x == 0) && (difference.y >= healingRange.lowerThreshold || difference.y == 0)
				&&
				(difference.x <= healingRange.upperThreshold || difference.x == 0) && (difference.y <= healingRange.upperThreshold || difference.y == 0))){

				healingAmount = healingRange.healingAmount;
			}
		}

		if (healingAmount == 0){
			return OUT_OF_RANGE + std::to_string(world->masterManager.unitLoader->customClasses.at(unitID).healingRangeValues[0].upperThreshold);
		}
		else if (_unit == this){
			return CANNOT_HEAL_SELF;
		}
		else if (hasHealed){
			return ALREADY_HEALED;
		}
		else if (_unit->gethp() == _unit->getMaxHp()){
			return AT_MAX_HP;
		}
		else {
			float hp = _unit->gethp();
			float max = _unit->getMaxHp();
			float hpDifference = max - hp;

			hasHealed = true;

			if (hpDifference >= healingAmount){
				return _unit->beHealed(healingAmount);
			}
			else{
				return _unit->beHealed(hpDifference);
			}
		}
	}

	return CANNOT_HEAL;
}

std::string UnitTile::beHealed(float num){
	hp += num;
	updateStats();
	std::string str;
	str = HEAL_SUCCESS + roundFloat(num);
	return str;
}

std::string UnitTile::attack(TerrainTile* _terrain){
    UnitTile* unit = _terrain->getUnit();

    std::string resultStr{};

	bool validMovDirection{false};
	bool validAttackDirection{false};
	bool rangedObstructionPresent{false};
	bool meleeObstructionPresent{false};
	bool inMovementRange{false};
	bool inRangedAttackRange{false};

	sf::Vector2i vectorDist = distanceFrom(_terrain, validMovDirection, validAttackDirection, rangedObstructionPresent, meleeObstructionPresent, inMovementRange, inRangedAttackRange);
	int dist{0};

	if (dir == Direction::N || dir == Direction::S){
		dist = abs(vectorDist.y);
	}
	else{
		dist = abs(vectorDist.x);
	}

	if (getSquareFormationActive()){
		return SF_ACTIVE;
	}

	if (((dist > 1 && (!canRangedAttack())) || getHasMeleeAttacked())
        ||
        ((dist == 1 && (!canMeleeAttack())) || getHasRangedAttacked())){

		return{ALREADY_ATTACKED};
	}


	if (!validAttackDirection){
		return{INVALID_DIR_ATK};
	}


	if(dist > 1){
        if(rangedObstructionPresent){
            return{OBSTRUCTION_PRESENT_ATK};
        }

        if(dist > getMaxRange()){
            return{OUT_OF_RANGE + std::to_string(getMaxRange())};
        }

	}

	else if (dist == 1){

	    //A little hardcoded check is made here. Specifically, engineers need to be able to "attack" water,
	    //which would normally be impossible because water counts as a melee obstruction, to construct bridges

        if(meleeObstructionPresent){
            if (!(_terrain->getTerrainType() == TerrainTile::TerrainType::WATER && unitType == UnitType::SAP)){
                    return{OBSTRUCTION_PRESENT_ATK};
               }
        }

        if(!getMelee()){
            return NO_MELEE;
        }
	}


	//Past this point, it is assumed combat is possible

	//Because we don't want terrain attacks to further increment rangedAttacks or meleeAttacks
	//if we attack a unit, we'll store variables for their success here.

	bool meleeSuccess{false};
	bool rangedSuccess{false};

	////////////////////////////////////////////////////////////////////////////

    if(unit != nullptr){
	//Terrain modifiers
	////////////////////////////////////////////////////////////////////////////
	this->applyTerrainModifiers(this->getTerrain(), dist, true);
	unit->applyTerrainModifiers(unit->getTerrain(), dist, false);
	////////////////////////////////////////////////////////////////////////////

	// Ranged
	if (dist > 1){
		//Generally, the rule is that in melee combat units only get modifiers according to the
		//current terrain tile they are standing on. When it comes to ranged, however, some terrain
		//tiles provide a malus to the shooter. Therefore, we call applyTerrainModifiers() on the
		//attacking unit, using the defender's tile as an argument.

		this->applyTerrainModifiers(unit->getTerrain(), dist, true);
		resultStr += this->rangedAttack(unit, dist);
		rangedSuccess = true;
	}

	//Melee
    else {
        UnitTile::Modifier flank{Modifier::FRONT_FLANK};

        //Determine flank direction

        if (this->getDir() == unit->getDir()){
            flank = Modifier::REAR_FLANK;
        }
        else if (this->getDir() == opposite(unit->getDir())){
            flank = Modifier::FRONT_FLANK;
        }
        else{
            flank = Modifier::SIDE_FLANK;
        }

        //Other Modifiers
        ////////////////////////////////////////////////////////////////////////////

        applyFlankModifier(flank, unit);

        this->applyBonusModifiers(unit, true);
        unit->applyBonusModifiers(this, false);

        ////////////////////////////////////////////////////////////////////////////

        //Double dispatch, hence the reverse order
        resultStr += unit->meleeAttack(this);
        meleeSuccess = true;
        }
    }

    //Attacking the terrain should always happen if the unit is
	//a. capable of doing so, and
	//b. the attack is ranged.

	if((unit == nullptr) || (unit != nullptr && dist > 1)){

        if (limber && hasLimberAbility()){
            resultStr += LIMBERED;
        }

        else{
            resultStr += terrainAttack(_terrain, dist);

            if(dist == 1 && !meleeSuccess){
                meleeAttacks++;
                mov = 0;
            }
            else if(dist > 1 && !rangedSuccess){
                rangedAttacks++;
                mov = 0;
            }
        }

	}

    return resultStr;
}


bool UnitTile::isHostile(UnitTile* _tile){
	if (player != _tile->getPlayer()){
		return true;
	}
	else{
		return false;
	}
}

void UnitTile::takeDamage(UnitTile* attacker, float& _dmg, int distance){

    if(attacker != nullptr){
        if (attacker->getFrightening() && distance == 1){
            _dmg += 1;
        }

        if (distance > 1 && getHalfRangedDamage() && _dmg >= 1){
            _dmg *= 0.5;
        }
    }

	hp -= _dmg;
	world->addToDamagedUnits(this);
}

bool UnitTile::removeIfDead(){
	if (hp < 0.4){
		player->removeUnit(this);
		return true;
	}
	return false;
}

void UnitTile::updateStats(bool randomisePerceivedPosition){

	//Update the stats
	dirText.setString(dirToString());
	movText.setString(std::to_string(mov));
	hpText.setString(roundFloat(hp));

    if(drawUnit){
        perceivedPosition = truePosition;
    }

    //////////////////////////////////////////////

    //Even if the we specify a false argument, if the flag is now in visual range (such as if a unit moved
    //close enough), we want to change its position.

    auto result = world->getVisibleTiles().find(world->terrainAtCartesianPos(perceivedPosition));

    if(result != world->getVisibleTiles().end()){
        randomisePerceivedPosition = true;
    }

    //If the unit is too far away, adjust the position of the sprite accordingly:
    if(!drawUnit && drawFlag && randomisePerceivedPosition){

        std::vector<TerrainTile*> surroundingTiles;

        for(int x{-1}; x <= 1; ++x){
            for (int y{-1}; y <= 1; ++y){

                TerrainTile* here = world->terrainAtCartesianPos({truePosition.x + x, truePosition.y + y});
                surroundingTiles.push_back(here);
            }
        }

        //After filling in surroundingTiles with the 8 surrounding tiles and true position, we then remove any
        //tiles in it that are either a). null, or b). in visual range of the current player

        for(auto i = surroundingTiles.begin(); i!= surroundingTiles.end();){

            if(*i == nullptr){
                i = surroundingTiles.erase(i);
            }

            else{
                auto found = world->getVisibleTiles().find(*i);

                if(found != world->getVisibleTiles().end()){
                    i = surroundingTiles.erase(i);
                }

                else{
                    ++i;
                }
            }

        }

        int maxSurroundingTilesIndex = surroundingTiles.size() - 1;

        //If there are no valid surrounding tiles, use the true position

        if(maxSurroundingTilesIndex < 0){
            perceivedPosition = truePosition;
        }

        else{
            boost::random::uniform_int_distribution<int> distribution(0, maxSurroundingTilesIndex);

            int randomIndex{distribution(world->masterManager.randomEngine)};
            perceivedPosition = surroundingTiles[randomIndex]->getCartesianPos();
        }

    }

    sf::Vector2f finalPosition = world->pixelPosAtCartesianPos(perceivedPosition);

    sprite.setPosition(finalPosition);
    unitFlag.setPosition(finalPosition);
    outline.setPosition(finalPosition);

	generalRange1.setPosition(getPixelPosCenter().x, bottom() - generalRange1.getTexture()->getSize().y);
	generalRange2.setPosition(getPixelPosCenter().x, bottom() - generalRange2.getTexture()->getSize().y);
	generalDead.setPosition(getPixelPosCenter().x, bottom() - generalDead.getTexture()->getSize().y);

    //////////////////////////////////////////////

	//Update the physical position of the stats, and sprite, if needed
    //This algorithm attempts to place the numbers in as even a space as possible:
    auto spriteSize = sprite.getLocalBounds();
	auto spritePos = sprite.getPosition();

    int combinedWidth = dirText.getLocalBounds().width + hpText.getLocalBounds().width + movText.getLocalBounds().width;
    int edgeSize = (spriteSize.width - combinedWidth) / 4;

    dirText.setPosition(spritePos.x + edgeSize, spritePos.y + spriteSize.height);
    movText.setPosition(spritePos.x + spriteSize.width - edgeSize - movText.getLocalBounds().width, spritePos.y + spriteSize.height);
    hpText.setPosition((dirText.getPosition().x + dirText.getLocalBounds().width + movText.getPosition().x) / 2 - hpText.getLocalBounds().width / 2 , spritePos.y + spriteSize.height);
}

void UnitTile::draw(sf::RenderTarget &target, sf::RenderStates states) const{

    if(drawUnit){
        target.draw(sprite, states);
        target.draw(dirText);
        target.draw(hpText);
        target.draw(movText);

        if (getHasAnyAttacked()){

            if(!canMeleeAttack() || !canRangedAttack()){
                outline.setOutlineColor(sf::Color::Red);
            }
            else{
                outline.setOutlineColor(sf::Color(255,140,0));
            }

        }
        else if (highlighted){
            outline.setOutlineColor(sf::Color::Yellow);
        }
        else if (squareFormationActive){
            outline.setOutlineColor(sf::Color(0,255,255));
        }
        else{
            outline.setOutlineColor(sf::Color::Transparent);
        }

        target.draw(outline);
    }

    if(drawFlag){
        target.draw(unitFlag);
    }

    if(drawGeneralRange2 && drawUnit){
        target.draw(generalRange2);
    }
    else if(drawGeneralRange1 && drawUnit){
        target.draw(generalRange1);
    }
    else if(drawGeneralDead && drawUnit){
        target.draw(generalDead);
    }

}

//This function returns a bool that determines whether the unit can cross a bridge or not.
//For example, if the unit is facing north and wants to cross a bridge, the bridge needs
//to have a southern connection. Conversely, if the unit is already standing on the bridge
//and wants to continue north, the bridge needs to have a northern connection.

bool UnitTile::appropriateBridgeConnectionPresent(Bridge* _bridge){
    if(dir == Direction::N){
		if(_bridge->getUnit() != this){
			return _bridge->southernConnection;
		}
		else{
			return _bridge->northernConnection;
		}
    }
    else if(dir == Direction::S){
		if(_bridge->getUnit() != this){
			return _bridge->northernConnection;
		}
		else{
			return _bridge->southernConnection;
		}
    }
    else if(dir == Direction::E){
		if(_bridge->getUnit() != this){
			return _bridge->westernConnection;
		}
		else{
			return _bridge->easternConnection;
		}
    }
    else if(dir == Direction::W){
		if(_bridge->getUnit() != this){
			return _bridge->easternConnection;
		}
		else{
			return _bridge->westernConnection;
		}
    }
}

sf::Vector2i UnitTile::distanceFrom(TerrainTile* _destinationTile, bool& _validMovDirection, bool& _validAttackDirection, bool& _rangedObstructionPresent, bool& _meleeObstructionPresent, bool& _inMovementRange, bool& _inRangedAttackRange){
    return distanceFrom(_destinationTile, _validMovDirection, _validAttackDirection, _rangedObstructionPresent, _meleeObstructionPresent, _inMovementRange, _inRangedAttackRange, canShootOverUnits, coneWidth);
}
/*
This function takes three optional parameters: canShootOverUnits, canShootAtCloseRange, coneWidth, which default to false,
false, and 1, respectively

canShootOverUnits specifies whether this unit has the ability to bypass LoS with regards to ranged attacking, and coneWidth specifies
the horizontal ranged attacking range this distance has. Most units (such as infantry) cannot shoot "past" other units, and can only
shoot straight forward. Artillery and mortar are excellent contrasting examples; they can shoot over units and have a 3-wide attacking
range.
*/

//virtual
sf::Vector2i UnitTile::distanceFrom(TerrainTile* _destinationTile, bool& _validMovDirection, bool& _validAttackDirection, bool& _rangedObstructionPresent, bool& _meleeObstructionPresent, bool& _inMovementRange, bool& _inRangedAttackRange, bool _canShootOverUnits, int _coneWidth){
	//coneWidth represents the width units can fire at. It should always be an odd number; 1 for the center, and 2/4/6 etc. for the sides

	//Exclude the center
	_coneWidth /= 2;

	sf::Vector2i initialCoords{terrain->getCartesianPos()};
	sf::Vector2i finalCoords{_destinationTile->getCartesianPos()};

	bool destinationIsUnit = (world->unitAtTerrain(_destinationTile) != nullptr);

	int distance{0};
	std::vector<TerrainTile*> tilesInBetween = world->getTerrainTilesBetween(terrain, _destinationTile, dir);
	distance = tilesInBetween.size();

	if(distance == 0){
		return {finalCoords - initialCoords};
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//Anything past this point assumes the distance is > 0;
	//i.e. that the destination tile is at least valid in
	//terms of direction.

	//We need to do some further checks to set the bools.
	////////////////////////////////////////////////////////////////////////////////////////

	_validAttackDirection = true;
	_validMovDirection = true;

	bool withinConeWidth{false};

	if(dir == Direction::E || dir == Direction::W){
        if((initialCoords.y >= finalCoords.y - _coneWidth) && (initialCoords.y <= finalCoords.y + _coneWidth)){
			withinConeWidth = true;
        }
	}
	else if(dir == Direction::S || dir == Direction::N){
        if((initialCoords.x >= finalCoords.x - _coneWidth) && (initialCoords.x <= finalCoords.x + _coneWidth)){
			withinConeWidth = true;
        }
	}

	if(withinConeWidth){
		if(distance <= getMaxRange()){
			_inRangedAttackRange = true;
		}
	}

	if(distance <= mov){
		_inMovementRange = true;
	}

	//A unit's destination tile needs to both be able to be crossable and stoppable in.
	if(!_destinationTile->unitCanCrossHere(this) || !_destinationTile->unitCanStopHere(this)){
		_meleeObstructionPresent = true;
	}

	for(auto& tile : tilesInBetween){
		//If obstructions have already been found, no need to keep searching, just exit loop
		if (_rangedObstructionPresent && _meleeObstructionPresent){
			continue;
		}

		//In the case the destination tile contains a unit, there should be no melee obstruction registered,
		//as the current unit should be able to attack it.
		//However, if the destination tile doesn't contain a unit, this means
		//....to fix.
		if(!destinationIsUnit){
			if(tile->getTerrainType() == TerrainTile::TerrainType::BRIDGE || tile->getTerrainType() == TerrainTile::TerrainType::TBRIDGE){
				Bridge* bridge = static_cast<Bridge*>(tile);
				bool validDirection{appropriateBridgeConnectionPresent(bridge)};
				if(!validDirection){
					_meleeObstructionPresent = true;
				}
			}
		}
		//If there's a unit in the way, check if it's an obstruction.
		//Also, check if the unit can cross the terrain here

		UnitTile* unitInTheWay{tile->getUnit()};


		//We don't want this part to be included for the destination tile
		if(tile != tilesInBetween.back()){
			if (unitInTheWay != nullptr){
				bool unitIsFriendly{unitInTheWay->getPlayer() == this->getPlayer()};

				if (!unitIsFriendly && unitInTheWay->drawUnit){
					_meleeObstructionPresent = true;
				}

				if (!_canShootOverUnits && unitInTheWay->drawUnit){
					_rangedObstructionPresent = true;
				}
			}

			if(!tile->unitCanCrossHere(this)){
				_meleeObstructionPresent = true;
			}
		}
	}

	return {finalCoords - initialCoords};
}

int UnitTile::distanceFrom(Tile* _tile){
	sf::Vector2i initialCoords{world->cartesianPosAtIndex(world->indexAtTile(*terrain))};
	sf::Vector2i finalCoords{world->cartesianPosAtIndex(world->indexAtTile(*_tile))};

	switch (dir){
	case UnitTile::Direction::N:
		return (initialCoords.y - finalCoords.y);

	case UnitTile::Direction::E:
		return (finalCoords.x - initialCoords.x);

	case UnitTile::Direction::S:
		return (finalCoords.y - initialCoords.y);

	case UnitTile::Direction::W:
		return (initialCoords.x - finalCoords.x);
	}

    return 0;
}


std::string UnitTile::attackReport(int distance, UnitTile* attacker, UnitTile* defender, int attackerRoll, int defenderRoll, float attackerInflicted, float defenderInflicted, bool retreat){
	std::stringstream result{};

	std::vector<ModifierReport>& attackerModifiers = attacker->modVector;
	std::vector<ModifierReport>& defenderModifiers = defender->modVector;

	std::string attackerRollString{};

	if (!attackerModifiers.empty()){

		float finalAttackerRoll{float(attackerRoll)};

		this->multRollByModifiers(finalAttackerRoll);

		attackerRollString = attacker->getPlayer()->getShortDisplayName().substr(0, 3) + " " + std::to_string(attackerRoll) + " -> " + roundFloat(finalAttackerRoll);
	}
	else{
		attackerRollString = attacker->getPlayer()->getShortDisplayName().substr(0, 3) + " " + std::to_string(attackerRoll);
	}

	std::string defenderRollString{};

	if (!defenderModifiers.empty()){
		float finalDefenderRoll{float(defenderRoll)};

		defender->multRollByModifiers(finalDefenderRoll);

		defenderRollString = defender->getPlayer()->getShortDisplayName().substr(0, 3) + " " + std::to_string(defenderRoll) + " -> " + roundFloat(finalDefenderRoll);
	}
	else{
		defenderRollString = defender->getPlayer()->getShortDisplayName().substr(0, 3) + " " + std::to_string(defenderRoll);
	}

	std::string attackerInflictedString{attacker->getPlayer()->getShortDisplayName().substr(0, 3) + " -" + roundFloat(defenderInflicted)};
	std::string defenderInflictedString{defender->getPlayer()->getShortDisplayName().substr(0, 3) + " -" + roundFloat(attackerInflicted)};

	result << "Combat:    " << attacker->getPlayer()->getShortDisplayName().substr(0, 3) + " " + attacker->getShortDisplayName() + "[" + attacker->dirToString() + "] " << " vs. " << defender->getPlayer()->getShortDisplayName().substr(0, 3) + " " + defender->getShortDisplayName() + "[" + defender->dirToString() + "]" << "; ";

	if (distance == 1){
		result << "melee";
	}
	else{
		result << "dist. " << std::to_string(distance);
	}

	result << std::endl;

	result << "Rolls:     ";

	//Using > 0.0.1 instead of != 0 because of float quirks

	if (attackerRoll > 0.01){
		result << attackerRollString;
	}

	if (attackerRoll > 0.01 && defenderRoll > 0.01){
		result << " | ";
	}

	if (defenderRoll > 0.01){
		result << defenderRollString;
	}

	result << "\n";
	result << "HP:        ";

	if (defenderInflicted > 0.01){
		result << attackerInflictedString;

		//If the HP is less than 0.5, it means they are about to be deleted due to death...

		if (attacker->gethp() < 0.4){
			result << " (killed)";
			if (attacker->getUnitType() == UnitTile::UnitType::GEN){
				result << "; -2HP to all " + attacker->getPlayer()->getShortDisplayName().substr(0, 3) + "!";
			}
		}
	}

	if (defenderInflicted > 0.01 && attackerInflicted > 0.01){
		result << " | ";
	}

	if (attackerInflicted > 0.01){
		result << defenderInflictedString;

		if (distance > 1 && defender->getHalfRangedDamage()){
			result << " (half ranged dmg) ";
		}

		//If the HP is less than 0.5, it means they are about to be deleted due to death...

		if (defender->gethp() < 0.4){
			result << " (killed)";
			if (defender->getUnitType() == UnitTile::UnitType::GEN){
				result << "; -2HP to all " + defender->getPlayer()->getFactionID().substr(0, 3) + "!";
			}
		}
	}

	result << "\n";


	result << attacker->getPlayer()->getShortDisplayName().substr(0, 3) + " Mod:   ";

	for (auto& mod : attackerModifiers){
		if (mod.modFloat != 0){
			if (mod.additional){
				std::string append{mod.modFloat >= 0 ? "+" : ""};
				result << "[" + modToString(mod) + ": " + append + roundFloat(mod.modFloat) + "]";
			}
			else{
				result << "[" + modToString(mod) + ": " + roundFloat(mod.modFloat) + "d]";
			}
		}
	}

	if (attacker->getFrightening() && distance == 1 && attackerInflicted > 0){
		result << "[frightening: +1DMG]";
	}

	result << "\n";

	result << defender->getPlayer()->getShortDisplayName().substr(0, 3) + " Mod:   ";

	if (retreat){
		result << "(retreat!) ";
	}

	for (auto& mod : defenderModifiers){
		if (mod.modFloat != 0){
			if (mod.additional){
				std::string append{mod.modFloat >= 0 ? "+" : ""};
				result << "[" + modToString(mod) + ": " + append + roundFloat(mod.modFloat) + "]";
			}
			else{
				result << "[" + modToString(mod) + ": " + roundFloat(mod.modFloat) + "d]";
			}
		}
	}

	if (defender->getFrightening() && distance == 1 && defenderInflicted > 0){
		result << "[frightening: +1DMG]";
	}

    result << std::endl;
	return result.str();
}

void UnitTile::multRollByModifiers(float &originalRoll){
	for (auto& mod : modVector){
		if (mod.modFloat != 0 && !mod.additional){
				originalRoll *= mod.modFloat;
		}
	}


	for (auto& mod : modVector){
		if (mod.modFloat != 0 && mod.additional){
			originalRoll += mod.modFloat;
		}
	}

	//If the roll goes below 1, pop it up back to 1
	if (originalRoll < 1.001){
		originalRoll = 1;
	}
}

void UnitTile::calculateEffectiveMov(){
	UnitTile* general = player->getGeneral();

	if (general == nullptr){
		//Implicitly rounded down
		setMov(getMaxMov()*0.50);
		drawGeneralDead = true;
        drawGeneralRange2 = false;
        drawGeneralRange1 = false;
	}
	else{
		sf::Vector2i generalPos = general->getCartesianPos();
		sf::Vector2i currentPos = getCartesianPos();
		sf::Vector2i distance = currentPos - generalPos;
		distance.x = abs(distance.x);
		distance.y = abs(distance.y);

		/*
		The values here are 21 and 31

        Because they are radii, however, we half them (and subtract 1 to
        exclude the tile the general is on themselves)

        We therefore get 10 and 15 respectively.

        */

		if (distance.x > 10 || distance.y > 10){
			if (distance.x <= 15 && distance.y <= 15){
				setMov(getMaxMov()*0.75);
				drawGeneralDead = false;
				drawGeneralRange1 = true;
				drawGeneralRange2 = false;
			}
			else{
				setMov(getMaxMov()*0.65);
				drawGeneralDead = false;
				drawGeneralRange1 = false;
				drawGeneralRange2 = true;
			}
		}
		else{
			setMov(getMaxMov());
			drawGeneralDead = false;
            drawGeneralRange2 = false;
            drawGeneralRange1 = false;
		}

	}
}

std::string UnitTile::rangedAttack(UnitTile* unit, int distance){

	if (getSquareFormationActive() && hasSquareFormationAbility()){
		return SF_ACTIVE;
	}

	if (limber && hasLimberAbility()){
		return LIMBERED + "\n";
	}

	boost::random::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(world->masterManager.randomEngine)};

	float thisRoll = thisRoll_int;

	float damageDealt{0};

	float distanceModifier{0};

	int lowerDieThreshold{1};
	int upperDieThreshold{6};
	bool modifierIsDamage{false};

	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	sf::Vector2i currentCoords = getCartesianPos();
	sf::Vector2i enemyCoords = unit->getCartesianPos();

	UnitTile* originalUnit = unit;

	int PRIMARYAXIS_CURRENT;
    int PRIMARYAXIS_DESTINATION;
    int SECONDARYAXIS_CURRENT;
    //Declared but unused; commented out in case I'll need it in the future
    //int SECONDARYAXIS_DESTINATION;

    int PRIMARYAXIS_MOVEMENT;

    //Our cone width can vary, but we only want to check the path along a single axis.
    //For example, with a unit (U) shooting with a cone width of 5, and an enemy (E),
    //the dashed path is the one that will be checked:
    //......................
    //..........E...........
    //..........-...........
    //..........-...........
    //..........-...........
    //..........-...........
    //..........-...........
    //........U.............
    //
    //We already know that this function won't be called if the enemy is not in
    //the cone width.
    //
    //Therefore, we set the secondary axis coordinate (in this case, the X axis)
    //of our unit to the same one as the enemy

    switch (dir){
        case UnitTile::Direction::N:
            PRIMARYAXIS_CURRENT = currentCoords.y;
            PRIMARYAXIS_DESTINATION = enemyCoords.y;
            SECONDARYAXIS_CURRENT = enemyCoords.x;
            //SECONDARYAXIS_DESTINATION = enemyCoords.x;

            PRIMARYAXIS_MOVEMENT = -1;
            break;

        case UnitTile::Direction::E:
            PRIMARYAXIS_CURRENT = currentCoords.x;
            PRIMARYAXIS_DESTINATION = enemyCoords.x;
            SECONDARYAXIS_CURRENT = enemyCoords.y;
            //SECONDARYAXIS_DESTINATION = enemyCoords.y;

            PRIMARYAXIS_MOVEMENT = 1;
            break;

        case UnitTile::Direction::S:
            PRIMARYAXIS_CURRENT = currentCoords.y;
            PRIMARYAXIS_DESTINATION = enemyCoords.y;
            SECONDARYAXIS_CURRENT = enemyCoords.x;
            //SECONDARYAXIS_DESTINATION = enemyCoords.x;

            PRIMARYAXIS_MOVEMENT = 1;
            break;


        case UnitTile::Direction::W:
            PRIMARYAXIS_CURRENT = currentCoords.x;
            PRIMARYAXIS_DESTINATION = enemyCoords.x;
            SECONDARYAXIS_CURRENT = enemyCoords.y;
            //SECONDARYAXIS_DESTINATION = enemyCoords.y;
            PRIMARYAXIS_MOVEMENT = -1;

            break;
        }

        sf::Vector2i finalCoords{enemyCoords};

        //Begin at the tile that comes after the current tile, and loop to the destination (inclusive)
        for (int i{PRIMARYAXIS_CURRENT + PRIMARYAXIS_MOVEMENT}; i != PRIMARYAXIS_DESTINATION; i += PRIMARYAXIS_MOVEMENT){

            TerrainTile* terrainInTheWay;

            if (dir == Direction::N || dir == Direction::S){
                terrainInTheWay = world->terrainAtCartesianPos({SECONDARYAXIS_CURRENT, i});
            }
            else{
                terrainInTheWay = world->terrainAtCartesianPos({i, SECONDARYAXIS_CURRENT});
            }

            UnitTile* unitInTheWay = terrainInTheWay->getUnit();

            if(unitInTheWay != nullptr){

                if(unitInTheWay->getPlayer() != getPlayer() && !unitInTheWay->drawUnit && !getCanShootOverUnits()){
                    unit = unitInTheWay;
                }
            }
        }

        bool intercepted{false};

        if(unit != originalUnit){
            intercepted = true;
            distance = distanceFrom(unit);
        }

        /////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////

	for (auto& item : world->masterManager.unitLoader->customClasses.at(unitID).rangedAttackDistValues){
		if (distance >= item.lowerThreshold && distance <= item.upperThreshold){
			distanceModifier = item.distModifier;
			modifierIsDamage = item.modifierIsDamage;
			lowerDieThreshold = item.lowerDieThreshold;
			upperDieThreshold = item.upperDieThreshold;
			continue;
		}
	}

	if (!modifierIsDamage){
		modVector.emplace_back(Modifier::DISTANCE, distanceModifier, false);

		multRollByModifiers(thisRoll);
		damageDealt += thisRoll;
	}
	else{
		multRollByModifiers(thisRoll);
		damageDealt = distanceModifier;
	}

	if (thisRoll_int >= lowerDieThreshold && thisRoll_int <= upperDieThreshold){
		unit->takeDamage(this, damageDealt, distance);
	}
	else{
		damageDealt = 0;
	}

	mov = 0;
	this->updateStats();
	unit->updateStats();
	rangedAttacks++;


	/*
	Units that can skirmish need to be able to rotate a second time sometimes. Take the following scenario, for instance:
	A LINF is facing south.
		-It turns north
		-It fires at an enemy
	Its skirmisher bonus should be usable now, and it should be able to turn south again.
	*/

	if (getSkirmish()){
		hasFullRotated = false;
	}

    std::string finalStr{};

    if(unit->drawUnit){
        finalStr = attackReport(distance, this, unit, thisRoll_int, 0, damageDealt, 0);
    }
    else if(intercepted){
        finalStr = "An unseen enemy in the way took the damage!\n";
    }

    return finalStr;
}

std::string UnitTile::terrainAttack(TerrainTile* terrain, int distance){
    return terrain->callTerrainAttack(this,distance);
}

std::string UnitTile::terrainAttack(Bridge* bridge, int distance){

    int maxRange{0};

    if(!world->masterManager.unitLoader->customClasses.at(unitID).bridgeAttackDistValues.empty()){
        maxRange = world->masterManager.unitLoader->customClasses.at(unitID).bridgeAttackDistValues[0].upperThreshold;
    }

	if(distance > maxRange){
        return {OUT_OF_RANGE_TERRAIN + std::to_string(maxRange)};
	}

    if(!canAttackBridge()){
        return {"Cannot attack bridges"};
    }

    if (getSquareFormationActive() && hasSquareFormationAbility()){
		return SF_ACTIVE;
	}

	if (limber && hasLimberAbility()){
		return LIMBERED;
	}

	boost::random::uniform_int_distribution<int> distribution(1, 6);
	int thisRoll_int{distribution(world->masterManager.randomEngine)};
	float thisRoll = thisRoll_int;

	float damageDealtF{0};
	float distanceModifier{0};

	int lowerDieThreshold{0};
	int upperDieThreshold{0};
	bool modifierIsDamage{false};

	for (auto& item : world->masterManager.unitLoader->customClasses.at(unitID).bridgeAttackDistValues){
		if (distance >= item.lowerThreshold && distance <= item.upperThreshold){
			distanceModifier = item.distModifier;
			modifierIsDamage = item.modifierIsDamage;
			lowerDieThreshold = item.lowerDieThreshold;
			upperDieThreshold = item.upperDieThreshold;
			continue;
		}
	}

	if (!modifierIsDamage){
		modVector.emplace_back(Modifier::DISTANCE, distanceModifier, false);

		multRollByModifiers(thisRoll);
		damageDealtF += thisRoll;
	}
	else{
		multRollByModifiers(thisRoll);
		damageDealtF = distanceModifier;
	}

	int damageDealtI = floor(damageDealtF);

	if (thisRoll_int >= lowerDieThreshold && thisRoll_int <= upperDieThreshold){
		bridge->takeDamage(damageDealtI);
	}
	else{
		damageDealtI = 0;
	}

	mov = 0;
	this->updateStats();


	/*
	Units that can skirmish need to be able to rotate a second time sometimes. Take the following scenario, for instance:
	A LINF is facing south.
		-It turns north
		-It fires at an enemy
	Its skirmisher bonus should be usable now, and it should be able to turn south again.
	*/

	if (getSkirmish()){
		hasFullRotated = false;
	}

    if(damageDealtI > 0){
        return{"Attacked bridge"};
    }

    return{};
}

std::string UnitTile::terrainAttack(TBridge* bridge, int distance){

    int maxRange{0};

    if(!world->masterManager.unitLoader->customClasses.at(unitID).bridgeAttackDistValues.empty()){
        maxRange = world->masterManager.unitLoader->customClasses.at(unitID).bridgeAttackDistValues[0].upperThreshold;
    }

	if(distance > maxRange){
        return {OUT_OF_RANGE_TERRAIN + std::to_string(maxRange)};
	}


    if(!canAttackBridge()){
        return {"Cannot attack bridges"};
    }

    if (getSquareFormationActive() && hasSquareFormationAbility()){
		return SF_ACTIVE;
	}

	if (limber && hasLimberAbility()){
		return LIMBERED;
	}

	boost::random::uniform_int_distribution<int> distribution(1, 6);
	int thisRoll_int{distribution(world->masterManager.randomEngine)};
	float thisRoll = thisRoll_int;

	float damageDealtF{0};
	float distanceModifier{0};

	int lowerDieThreshold{0};
	int upperDieThreshold{0};
	bool modifierIsDamage{false};

	for (auto& item : world->masterManager.unitLoader->customClasses.at(unitID).bridgeAttackDistValues){
		if (distance >= item.lowerThreshold && distance <= item.upperThreshold){
			distanceModifier = item.distModifier;
			modifierIsDamage = item.modifierIsDamage;
			lowerDieThreshold = item.lowerDieThreshold;
			upperDieThreshold = item.upperDieThreshold;
			continue;
		}
	}

	if (!modifierIsDamage){
		modVector.emplace_back(Modifier::DISTANCE, distanceModifier, false);

		multRollByModifiers(thisRoll);
		damageDealtF += thisRoll;
	}
	else{
		multRollByModifiers(thisRoll);
		damageDealtF = distanceModifier;
	}

	int damageDealtI = floor(damageDealtF);

	if (thisRoll_int >= lowerDieThreshold && thisRoll_int <= upperDieThreshold){
		bridge->takeDamage(damageDealtI);
	}
	else{
		damageDealtI = 0;
	}

	mov = 0;
	this->updateStats();

	/*
	Units that can skirmish need to be able to rotate a second time sometimes. Take the following scenario, for instance:
	A LINF is facing south.
		-It turns north
		-It fires at an enemy
	Its skirmisher bonus should be usable now, and it should be able to turn south again.
	*/

	if (getSkirmish()){
		hasFullRotated = false;
	}

    if(damageDealtI > 0){
        return{"Attacked bridge"};
    }

    return{};
}


UnitTile::~UnitTile(){
	//Required for this to be an abstract class
}
