#include "stdafx.h"
#include "Artillery.h"

#include "World.h"

Artillery::Artillery(World& _world, std::mt19937& _mt19937, Player* _belongsToPlayer, TextureManager& tm, FontManager& fm, UnitTile::Direction _dir) :
UnitTile(_world, _mt19937, _belongsToPlayer, tm, fm, TextureManager::Unit::ART, UnitType::ART, UnitFamily::ART_FAMILY, _dir)
{
	mov = maxMov;
	hp = maxhp;

	rangedAttackDistValues.emplace_back(20, 24, 1, true, 4, 6);
	rangedAttackDistValues.emplace_back(10, 19, 2, true, 4, 6);
	rangedAttackDistValues.emplace_back(2, 9, 4, true, 4, 6);
}

void Artillery::reset(){
	calculateEffectiveMov();	

	if (!limber){
		mov = 0;
	}

	hasPartialRotated = false;
	hasFullRotated = false;
	hasMeleeAttacked = false;
	hasRangedAttacked = false;
	hasMoved = false;
	updateStats();
}

std::string Artillery::rotate(UnitTile::Direction _dir){
	bool oppositeRotation{_dir == opposite(dir)};

	if (hasRangedAttacked){
		return NO_ROTATE_AFTER_ATTACK;
	}
	else if (getHasAnyRotated()){
		return ALREADY_ROTATED;
	}
	else if (dir == _dir){
		return ALREADY_FACING + UnitTile::dirToString();
	}

	if (oppositeRotation){
		hasFullRotated = true;
	}
	else{
		hasPartialRotated = true;
	}

	dir = _dir;
	updateStats();

	return SUCCESSFUL_ROTATION + UnitTile::dirToString();
}

sf::Vector2i Artillery::distanceFrom(TerrainTile* _terrain, bool& _validMovDirection, bool& _validAttackDirection, bool& _obstructionPresent, bool& _inMovementRange, bool& _inRangedAttackRange, bool canShootOverUnits, int coneWidth){
	return UnitTile::distanceFrom(_terrain, _validMovDirection, _validAttackDirection, _obstructionPresent, _inMovementRange, _inRangedAttackRange, true, 5);
}

std::string Artillery::meleeAttack(UnitTile* _unit){
	//This is used for double dispatch calls. However, we can also place the code for artillery's guard protection
	//here, since this will always be called when a unit meleeAttacks()'s artillery.

	bool protectedByGuard{false};
	sf::Vector2i currentPos = getCartesianPos();

	//Check all adjacent tiles for a friendly artillery guard
	for (int x{-1}; x <= 1 && !protectedByGuard; ++x){
		for (int y{-1}; y <= 1 && !protectedByGuard; ++y){

			sf::Vector2i adjacentPos{currentPos.x + x, currentPos.y + y};
			auto unit = world.unitAtTerrain(world.terrainAtCartesianPos(adjacentPos));

			if (unit != nullptr){

				if (unit->getPlayer() == getPlayer() && unit->getUnitType() == UnitType::ARTGUARD){
					protectedByGuard = true;
				}
			}
		}

	}

	if (!protectedByGuard){
		return _unit->meleeAttack(this);
	}

	return ARTILLERY_GUARD;
}

std::string Artillery::rangedAttack(UnitTile* unit, int distance){

	if (limber){
		return LIMBERED;
	}

	return UnitTile::rangedAttack(unit, distance);
}

void Artillery::toggleLimber(){
	mov = 0;
	hasMeleeAttacked = true;
	hasRangedAttacked = true;
	hasMoved = true;

	limber = !limber;

	updateStats();
}