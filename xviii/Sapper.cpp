#include "stdafx.h"
#include "Sapper.h"


Sapper::Sapper(World& _world, std::mt19937& _mt19937, Player* _belongsToPlayer, TextureManager& tm, FontManager& fm, UnitTile::Direction _dir) :
Infantry(_world, _mt19937, _belongsToPlayer, tm, fm, _dir, TextureManager::Unit::SAP, UnitTile::UnitType::SAP)
{
	deploymentCost = 1;
	limit = 0;

	
	mov = maxMov;
	hp = maxhp;
}

std::string Sapper::rangedAttack(UnitTile* unit, int distance){
	std::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(mt19937)};

	float thisRoll = thisRoll_int;

	float damageDealt{0};

	float distanceModifier{1};

	if (distance >= 3 && distance <= 4){
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