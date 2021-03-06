#include "xviii/Headers/stdafx.h"
#include "xviii/Units/Artillery.h"

#include "xviii/Core/World.h"
#include "xviii/Core/UnitLoader.h"

Artillery::Artillery(World* _world, Player* _belongsToPlayer, std::string _unitID, UnitType _type, UnitFamily _familyType, Direction _dir) :
UnitTile(_world, _belongsToPlayer, _unitID, _type, _familyType, _dir)
{
}

std::string Artillery::rotate(UnitTile::Direction _dir){
	bool oppositeRotation{_dir == opposite(dir)};

	if (getHasRangedAttacked()){
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

std::string Artillery::meleeAttack(UnitTile* _unit){
	//This is used for double dispatch calls. However, we can also place the code for artillery's guard protection
	//here, since this will always be called when a unit meleeAttacks()'s artillery.

	bool protectedByGuard{false};
	sf::Vector2i currentPos = getCartesianPos();

	//Check all adjacent tiles for a friendly artillery guard
	for (int x{-1}; x <= 1 && !protectedByGuard; ++x){
		for (int y{-1}; y <= 1 && !protectedByGuard; ++y){

			sf::Vector2i adjacentPos{currentPos.x + x, currentPos.y + y};
			auto unit = world->unitAtTerrain(world->terrainAtCartesianPos(adjacentPos));

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
