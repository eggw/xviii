#include "stdafx.h"
#include "Infantry.h"

#include "Player.h"
#include "World.h"

const float infFrontFlankModifier = 0.5;
const float infSideFlankModifier = 1;
const float infRearFlankModifier = 2;

const float cavFrontFlankModifier = 1;
const float cavSideFlankModifier = 1.5;
const float cavRearFlankModifier = 2;


Infantry::Infantry(World& _world, std::mt19937& _mt19937, Player* _belongsToPlayer, TextureManager& tm, FontManager& fm, UnitTile::Direction _dir) :
UnitTile(_world, _mt19937, _belongsToPlayer, tm, fm, TextureManager::Unit::INF, UnitType::INF, _dir)
{
	deploymentCost = 1;
	limit = 0;

	mov = maxMov;
	hp = maxhp;
}

std::string Infantry::moveTo(TerrainTile* terrainTile){

	bool validMovDirection{false};
	bool validAttackDirection{false};
	bool obstructionPresent{false};
	bool inMovementRange{false};
	bool inRangedAttackRange{false};
	int movExpended{0};

	//Get the coordinates of the current tile the unit is at
	sf::Vector2i currentCoords{world.cartesianCoordsAtIndex(world.indexAtTile(*at))};

	//Get the coordinates of the tile to be moved to
	sf::Vector2i toMoveToCoords{world.cartesianCoordsAtIndex(world.indexAtTile(*terrainTile))};

	movExpended = distanceFrom(terrainTile, validMovDirection, validAttackDirection, obstructionPresent, inMovementRange, inRangedAttackRange);

	if (obstructionPresent){
		return "Invalid move order: line of sight not clear";
	}

	else if (validMovDirection && inMovementRange){
		hasRotated = true;
		at = terrainTile;
		mov -= movExpended;
		sprite.setPosition(terrainTile->getPos());
		unitFlag.setPosition(terrainTile->getPos());
		rekt.setPosition(terrainTile->getPos());
		updateStats();
		return "Successfully moved to (" + std::to_string(toMoveToCoords.x + 1) + ", " + std::to_string(toMoveToCoords.y + 1) + ")";
	}

	else if (validMovDirection && !inMovementRange){
		return{"Invalid move order: not enough mov"};
	}

	else if (!validMovDirection){
		return{"Invalid move order: hrzntl or wrong dir"};
	}

}


std::string Infantry::rotate(UnitTile::Direction _dir){
	if (hasRotated){
		return "Cannot rotate any more";
	}
	else if (hasMoved){
		return "Already moved this turn";
	}
	else if (dir == _dir){
		return "Already facing " + UnitTile::dirToString(dir);
	}

	hasRotated = true;
	mov = 0;
	dir = _dir;
	updateStats();

	return "Successfully rotated to " + UnitTile::dirToString(dir);
}

std::string Infantry::interactWithFriendly(UnitTile* _unit){
	return{};
}

int Infantry::getMaxHp() const{
	return maxhp;
}

int Infantry::getMaxMov() const{
	return maxMov;
}

int Infantry::getMaxRange() const{
	return maxRange;
}

std::string Infantry::attack(Infantry* inf, int distance, UnitTile::Modifier flank){

	if (distance != 1){
		return rangedAttack(inf, distance);
	}

	std::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(mt19937)};
	int enemyRoll_int{distribution(mt19937)};

	float thisRoll = thisRoll_int;
	float enemyRoll = enemyRoll_int;

	float damageDealt{0};
	float damageReceived{0};

	Modifier flankType;
	float flankModifier;

	switch (flank){
	case Modifier::FRONT_FLANK:
		flankType = Modifier::FRONT_FLANK;
		flankModifier = infFrontFlankModifier;
		break;

	case Modifier::SIDE_FLANK:
		flankType = Modifier::SIDE_FLANK;
		flankModifier = infSideFlankModifier;
		break;

	case Modifier::REAR_FLANK:
		flankType = Modifier::REAR_FLANK;
		flankModifier = infRearFlankModifier;
	}

	modVector.emplace_back(flankType, flankModifier);

	multRollByModifiers(thisRoll);
	inf->multRollByModifiers(enemyRoll);

	if (abs(thisRoll - enemyRoll) < 0.01){
		damageDealt = 1;
		damageReceived = 1;

		this->takeDamage(damageReceived);
		inf->takeDamage(damageDealt);
	}
	else{
		//If the difference between rolls is less than 3
		if (abs(thisRoll - enemyRoll) < 3){
			if (thisRoll > enemyRoll){
				damageDealt = 1;

				inf->takeDamage(damageDealt);
			}
			else if (enemyRoll > thisRoll){
				damageReceived = 1;

				this->takeDamage(damageReceived);
			}
		}
		//If the difference is greater or equal to 3,
		else{
			if (thisRoll > enemyRoll){
				damageDealt = 2;
				inf->takeDamage(damageDealt);
			}
			else if (enemyRoll > thisRoll){
				damageReceived = 2;
				this->takeDamage(damageReceived);
			}
		}
	}

	mov = 0;
	this->updateStats();
	inf->updateStats();
	hasAttacked = true;

	return attackReport(distance, this, inf, thisRoll_int, enemyRoll_int, damageDealt, damageReceived, modVector, inf->modVector);

}

std::string Infantry::attack(Cavalry* cav, int distance, UnitTile::Modifier flank){

	if (distance != 1){
		return rangedAttack(cav, distance);
	}

	std::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(mt19937)};
	int enemyRoll_int{distribution(mt19937)};

	float thisRoll = thisRoll_int;
	float enemyRoll = enemyRoll_int;

	float damageDealt{0};
	float damageReceived{0};


	Modifier flankType;
	float flankModifier;

	switch (flank){
	case Modifier::FRONT_FLANK:
		flankType = Modifier::FRONT_FLANK;
		flankModifier = cavFrontFlankModifier;
		break;

	case Modifier::SIDE_FLANK:
		flankType = Modifier::SIDE_FLANK;
		flankModifier = cavSideFlankModifier;
		break;

	case Modifier::REAR_FLANK:
		flankType = Modifier::REAR_FLANK;
		flankModifier = cavRearFlankModifier;
	}

	modVector.emplace_back(flankType, flankModifier);

	multRollByModifiers(thisRoll);
	cav->multRollByModifiers(enemyRoll);

	if (abs(thisRoll - enemyRoll) < 0.01){
		damageDealt = 1;
		damageReceived = 0.5;

		cav->takeDamage(damageDealt);
		this->takeDamage(damageReceived);
	}
	else if (thisRoll > enemyRoll){
		damageDealt = 2;
		damageReceived = 1;

		cav->takeDamage(damageDealt);
		this->takeDamage(damageReceived);

	}
	else if (enemyRoll > thisRoll){
		damageReceived = 4;

		this->takeDamage(damageReceived);

	}

	mov = 0;
	this->updateStats();
	cav->updateStats();
	hasAttacked = true;

	return attackReport(distance, this, cav, thisRoll_int, enemyRoll_int, damageDealt, damageReceived, modVector, cav->modVector);
	
}

std::string Infantry::attack(Cuirassier* cuir, int distance, UnitTile::Modifier flank){

	if (distance != 1){
		return rangedAttack(cuir, distance);
	}

	std::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(mt19937)};
	int enemyRoll_int{distribution(mt19937)};

	float thisRoll = thisRoll_int;
	float enemyRoll = enemyRoll_int;

	float damageDealt{0};
	float damageReceived{0};


	Modifier flankType;
	float flankModifier;

	switch (flank){
	case Modifier::FRONT_FLANK:
		flankType = Modifier::FRONT_FLANK;
		flankModifier = cavFrontFlankModifier;
		break;

	case Modifier::SIDE_FLANK:
		flankType = Modifier::SIDE_FLANK;
		flankModifier = cavSideFlankModifier;
		break;

	case Modifier::REAR_FLANK:
		flankType = Modifier::REAR_FLANK;
		flankModifier = cavRearFlankModifier;
	}

	modVector.emplace_back(flankType, flankModifier);

	multRollByModifiers(thisRoll);
	cuir->multRollByModifiers(enemyRoll);

	if (abs(thisRoll - enemyRoll) < 0.01){
		damageDealt = 1;
		damageReceived = 0.5;

		cuir->takeDamage(damageDealt);
		this->takeDamage(damageReceived);
	}
	else if (thisRoll > enemyRoll){
		damageDealt = 2;
		damageReceived = 1;

		cuir->takeDamage(damageDealt);
		this->takeDamage(damageReceived);

	}
	else if (enemyRoll > thisRoll){
		damageReceived = 4;

		this->takeDamage(damageReceived);

	}

	mov = 0;
	this->updateStats();
	cuir->updateStats();
	hasAttacked = true;

	return attackReport(distance, this, cuir, thisRoll_int, enemyRoll_int, damageDealt, damageReceived, modVector, cuir->modVector);

}

std::string Infantry::attack(Dragoon* drag, int distance, UnitTile::Modifier flank){

	if (distance != 1){
		return rangedAttack(drag, distance);
	}

	std::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(mt19937)};
	int enemyRoll_int{distribution(mt19937)};

	float thisRoll = thisRoll_int;
	float enemyRoll = enemyRoll_int;

	float damageDealt{0};
	float damageReceived{0};


	Modifier flankType;
	float flankModifier;

	switch (flank){
	case Modifier::FRONT_FLANK:
		flankType = Modifier::FRONT_FLANK;
		flankModifier = cavFrontFlankModifier;
		break;

	case Modifier::SIDE_FLANK:
		flankType = Modifier::SIDE_FLANK;
		flankModifier = cavSideFlankModifier;
		break;

	case Modifier::REAR_FLANK:
		flankType = Modifier::REAR_FLANK;
		flankModifier = cavRearFlankModifier;
	}

	modVector.emplace_back(flankType, flankModifier);

	multRollByModifiers(thisRoll);
	drag->multRollByModifiers(enemyRoll);

	if (abs(thisRoll - enemyRoll) < 0.01){
		damageDealt = 1;
		damageReceived = 0.5;

		drag->takeDamage(damageDealt);
		this->takeDamage(damageReceived);
	}
	else if (thisRoll > enemyRoll){
		damageDealt = 2;
		damageReceived = 1;

		drag->takeDamage(damageDealt);
		this->takeDamage(damageReceived);

	}
	else if (enemyRoll > thisRoll){
		damageReceived = 4;

		this->takeDamage(damageReceived);

	}

	mov = 0;
	this->updateStats();
	drag->updateStats();
	hasAttacked = true;

	return attackReport(distance, this, drag, thisRoll_int, enemyRoll_int, damageDealt, damageReceived, modVector, drag->modVector);

}

std::string Infantry::attack(LightCav* lcav, int distance, UnitTile::Modifier flank){

	if (distance != 1){
		return rangedAttack(lcav, distance);
	}

	std::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(mt19937)};
	int enemyRoll_int{distribution(mt19937)};

	float thisRoll = thisRoll_int;
	float enemyRoll = enemyRoll_int;

	float damageDealt{0};
	float damageReceived{0};


	Modifier flankType;
	float flankModifier;

	switch (flank){
	case Modifier::FRONT_FLANK:
		flankType = Modifier::FRONT_FLANK;
		flankModifier = cavFrontFlankModifier;
		break;

	case Modifier::SIDE_FLANK:
		flankType = Modifier::SIDE_FLANK;
		flankModifier = cavSideFlankModifier;
		break;

	case Modifier::REAR_FLANK:
		flankType = Modifier::REAR_FLANK;
		flankModifier = cavRearFlankModifier;
	}

	modVector.emplace_back(flankType, flankModifier);

	multRollByModifiers(thisRoll);
	lcav->multRollByModifiers(enemyRoll);

	if (abs(thisRoll - enemyRoll) < 0.01){
		damageDealt = 1;
		damageReceived = 0.5;

		lcav->takeDamage(damageDealt);
		this->takeDamage(damageReceived);
	}
	else if (thisRoll > enemyRoll){
		damageDealt = 2;
		damageReceived = 1;

		lcav->takeDamage(damageDealt);
		this->takeDamage(damageReceived);

	}
	else if (enemyRoll > thisRoll){
		damageReceived = 4;

		this->takeDamage(damageReceived);

	}

	mov = 0;
	this->updateStats();
	lcav->updateStats();
	hasAttacked = true;

	return attackReport(distance, this, lcav, thisRoll_int, enemyRoll_int, damageDealt, damageReceived, modVector, lcav->modVector);

}

std::string Infantry::attack(Artillery* art, int distance, UnitTile::Modifier flank){

	if (distance != 1){
		return rangedAttack(art, distance);
	}

	std::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(mt19937)};

	float damageDealt{0};
	float damageReceived{0};


	float thisRoll = thisRoll_int;

	
	multRollByModifiers(thisRoll);

	if (thisRoll > 3 || abs(thisRoll - 3) < 0.01){
		damageDealt = 3;

		art->takeDamage(damageDealt);

	}
	else if (thisRoll < 3){
		damageDealt = 3;
		damageReceived = 3;

		art->takeDamage(damageDealt);
		this->takeDamage(damageReceived);
	}

	mov = 0;
	this->updateStats();
	art->updateStats();
	hasAttacked = true;

	return attackReport(distance, this, art, thisRoll_int, 0, damageDealt, damageReceived, modVector, art->modVector);

}

std::string Infantry::attack(Mortar* mor, int distance, UnitTile::Modifier flank){

	if (distance != 1){
		return rangedAttack(mor, distance);
	}

	std::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(mt19937)};

	float damageDealt{0};
	float damageReceived{0};

	float thisRoll = thisRoll_int;

	multRollByModifiers(thisRoll);

	if (thisRoll > 3 || abs(thisRoll - 3) < 0.01){
		damageDealt = 2;

		mor->takeDamage(damageDealt);

	}
	else if (thisRoll < 3){
		damageDealt = 2;
		damageReceived = 0.5;

		mor->takeDamage(damageDealt);
		this->takeDamage(damageReceived);
	}


	mov = 0;
	this->updateStats();
	mor->updateStats();
	hasAttacked = true;

	return attackReport(distance, this, mor, thisRoll_int, 0, damageDealt, damageReceived, modVector, mor->modVector);

}

std::string Infantry::attack(General* gen, int distance, UnitTile::Modifier flank){

	if (distance != 1){
		return rangedAttack(gen, distance);
	}

	std::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(mt19937)};
	int enemyRoll_int{distribution(mt19937)};

	float thisRoll = thisRoll_int;
	float enemyRoll = enemyRoll_int;
	
	float damageDealt{0};
	float damageReceived{0};

	Modifier flankType;
	float flankModifier;

	switch (flank){
	case Modifier::FRONT_FLANK:
		flankType = Modifier::FRONT_FLANK;
		flankModifier = cavFrontFlankModifier;
		break;

	case Modifier::SIDE_FLANK:
		flankType = Modifier::SIDE_FLANK;
		flankModifier = cavSideFlankModifier;
		break;

	case Modifier::REAR_FLANK:
		flankType = Modifier::REAR_FLANK;
		flankModifier = cavRearFlankModifier;
	}

	modVector.emplace_back(flankType, flankModifier);

	multRollByModifiers(thisRoll);
	gen->multRollByModifiers(enemyRoll);

		
	if (abs(thisRoll - enemyRoll) < 0.01){
		damageDealt = 1;
		damageReceived = 0.5;

		gen->takeDamage(damageDealt);
		this->takeDamage(damageReceived);
	}
	else if (thisRoll > enemyRoll){
		damageDealt = 2;
		damageReceived = 1;

		gen->takeDamage(damageDealt);
		this->takeDamage(damageReceived);
	}
	else if (enemyRoll > thisRoll){
		damageReceived = 4;
		this->takeDamage(damageReceived);

	}

	mov = 0;
	this->updateStats();
	gen->updateStats();
	hasAttacked = true;

	return attackReport(distance, this, gen, thisRoll_int, enemyRoll_int, damageDealt, damageReceived, modVector, gen->modVector);

}

std::string Infantry::rangedAttack(UnitTile* unit, int distance){

	std::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(mt19937)};

	float thisRoll = thisRoll_int;

	float damageDealt{0};

	if (distance > maxRange){
		return outOfRange();
	}

	float distanceModifier;

	if (distance == 6){
		distanceModifier = 0.5;
	}
	else if (distance >= 3 && distance <= 5){
		distanceModifier = 1;
	}
	else if (distance == 2 || distance == 1){
		distanceModifier = 2;
	}

	modVector.emplace_back(Modifier::DISTANCE, distanceModifier);

	multRollByModifiers(thisRoll);
	damageDealt = thisRoll;
	unit->takeDamage(damageDealt);

	mov = 0;
	this->updateStats();
	unit->updateStats();
	hasAttacked = true;

	return attackReport(distance, this, unit, thisRoll_int, 0, damageDealt, 0, modVector, unit->modVector);
	
}