#include "stdafx.h"
#include "KhevKveitebi.h"


KhevKveitebi::KhevKveitebi(World& _world, std::mt19937& _mt19937, Player* _belongsToPlayer, TextureManager& tm, FontManager& fm, UnitTile::Direction _dir) : 
FootGuard(_world, _mt19937, _belongsToPlayer, tm, fm, _dir, TextureManager::Unit::HINF, UnitTile::UnitType::KKV)
{
	mov = maxMov;
	hp = maxhp;

	//They inherit from Footguards but aren't meant to heal; hence, clear healingRangeValues upon creation
	healingRangeValues.clear();
}

void KhevKveitebi::preMeleeAttack(UnitTile* unit, bool attacking){
	if (unit->getUnitFamilyType() == UnitFamily::CAV_FAMILY){
		this->modVector.emplace_back(Modifier::ADDITIONAL, -1);
	}

	//+1 in melee rolls
	this->modVector.emplace_back(Modifier::ADDITIONAL, 1);
}