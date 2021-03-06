#include "xviii/Headers/stdafx.h"
#include "xviii/Units/General.h"

#include "xviii/Core/Player.h"

General::General(World* _world, Player* _belongsToPlayer, std::string _unitID, UnitType _type, UnitFamily _familyType, Direction _dir) :
Cavalry(_world, _belongsToPlayer, _unitID, _type, _familyType, _dir)
{
}

void General::reset(){
	mov = getMaxMov();

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

void General::takeDamage(UnitTile* attacker, float& _dmg, int distance){
	if (hp - _dmg < 0.4){
		for (auto& unit : player->getUnits()){
			if (unit.get() != this){
				//It is neither ranged nor melee, really, hence the 0
				float boilerplate{2};
				unit->takeDamage(attacker, boilerplate, 0);
			}
		}
	}

	UnitTile::takeDamage(attacker, _dmg, distance);
}

