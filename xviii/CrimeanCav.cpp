#include "stdafx.h"
#include "CrimeanCav.h"

CrimeanCav::CrimeanCav(World& _world, std::mt19937& _mt19937, Player* _belongsToPlayer, TextureManager& tm, FontManager& fm, UnitTile::Direction _dir) :
LightCav(_world, _mt19937, _belongsToPlayer, tm, fm, _dir, TextureManager::Unit::LCAV, UnitType::CRICAV)
{
	deploymentCost = 3;
	limit = 5;

	
	mov = maxMov;
	hp = maxhp;
}

std::string CrimeanCav::rotate(UnitTile::Direction _dir){
	if (hasMeleeAttacked){
		return NO_ROTATE_AFTER_MELEE;
	}
	else if (hasRotated){
		return ALREADY_ROTATED;
	}
	else if (dir == _dir){
		return ALREADY_FACING + UnitTile::dirToString();
	}
	//If it was a full rotation
	if (_dir == opposite(dir)){
		//Due to the rule that cav cannot attack after full rotation, and to simplify matters, I set the
		//hasAttacked variables to true here
		hasMeleeAttacked = true;
		hasRangedAttacked = true;
		mov = 2;
	}

	hasRotated = true;
	dir = _dir;
	updateStats();
	
	return SUCCESSFUL_ROTATION + UnitTile::dirToString();
}

std::string CrimeanCav::rangedAttack(UnitTile* unit, int distance){
	std::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(mt19937)};

	float thisRoll = thisRoll_int;

	float damageDealt{0};

	float distanceModifier{1};

	if (distance >= 3 && distance <= 8){
		distanceModifier = 0.5;
	}
	else if (distance == 2){
		distanceModifier = 1;
	}

	modVector.emplace_back(Modifier::DISTANCE, distanceModifier);

	multRollByModifiers(thisRoll);
	damageDealt = thisRoll;
	unit->takeDamage(damageDealt);

	mov = 0;
	this->updateStats();
	unit->updateStats();
	hasRangedAttacked = true;

	return attackReport(distance, this, unit, thisRoll_int, 0, damageDealt, 0);
}