#include "stdafx.h"
#include "PersianInf.h"


PersianInf::PersianInf(World& _world, std::mt19937& _mt19937, Player* _belongsToPlayer, TextureManager& tm, FontManager& fm, UnitTile::Direction _dir) :
Infantry(_world, _mt19937, _belongsToPlayer, tm, fm, _dir, TextureManager::Unit::INF, UnitTile::UnitType::PINF)
{
	mov = maxMov;
	hp = maxhp;
}

void PersianInf::preMeleeAttack(UnitTile* unit, bool attacking){
	if (unit->getUnitFamilyType() == UnitFamily::CAV_FAMILY){
		this->modVector.emplace_back(Modifier::ADDITIONAL, -1);
	}
}