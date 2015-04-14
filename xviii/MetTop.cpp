#include "stdafx.h"
#include "MetTop.h"


MetTop::MetTop(World& _world, std::mt19937& _mt19937, Player* _belongsToPlayer, TextureManager& tm, FontManager& fm, UnitTile::Direction _dir) : 
Infantry(_world, _mt19937, _belongsToPlayer, tm, fm, _dir, TextureManager::Unit::INF, UnitTile::UnitType::METTOP)
{
	deploymentCost = 1;
	limit = 0;

	waterCrosser = false;
	mov = maxMov;
	hp = maxhp;
}

int MetTop::getMaxHp() const{
	return maxhp;
}

int MetTop::getMaxMov() const{
	return maxMov;
}

int MetTop::getMaxRange() const{
	return maxRange;
}