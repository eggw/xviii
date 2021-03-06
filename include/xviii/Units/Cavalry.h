#pragma once

#include "xviii/Units/UnitTile.h"

class Cavalry : public UnitTile
{
public:
	Cavalry(World* _world, Player* _belongsToPlayer, std::string _unitID, UnitType _type, UnitFamily _familyType, Direction _dir);

	//The lancer bonus, here for convenience. Certain subclasses of Cavalry are meant to have this. The boolean lancer
	//determines whether this code is run or not.
	bool lancerBonus(UnitTile* defender, float finalAttackerRoll, bool attackBonusReady, float& damageDealt);

	virtual std::string rotate(Direction _dir);

	virtual std::string meleeAttack(UnitTile* _unit);

	virtual std::string meleeAttack(Infantry* inf);
	virtual std::string meleeAttack(Cavalry* cav);
	virtual std::string meleeAttack(Artillery* art);
	virtual std::string meleeAttack(Mortar* mor);
};

