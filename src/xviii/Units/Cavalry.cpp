#include "xviii/Headers/stdafx.h"
#include "xviii/Units/Cavalry.h"

#include "xviii/Core/Player.h"

bool Cavalry::lancerBonus(UnitTile* defender, float finalAttackerRoll, bool attackBonusReady, float& damageDealt){
	//If FINAL roll is 8 or above and attack bonus is ready...
	if (finalAttackerRoll >= 8 && attackBonusReady){
		if (defender->getUnitFamilyType() == UnitFamily::INF_FAMILY
			|| defender->getUnitFamilyType() == UnitFamily::HINF_FAMILY
			|| defender->getUnitFamilyType() == UnitFamily::LINF_FAMILY){
			damageDealt += 4;
		}
		else if (defender->getUnitFamilyType() == UnitFamily::CAV_FAMILY){
			damageDealt += 3;
		}

		defender->takeDamage(this, damageDealt, 1);
		return true;
	}

	return false;
}

Cavalry::Cavalry(World* _world, Player* _belongsToPlayer, std::string _unitID, UnitType _type, UnitFamily _familyType, Direction _dir) :
UnitTile(_world, _belongsToPlayer, _unitID, _type, _familyType, _dir)
{
}

std::string Cavalry::rotate(UnitTile::Direction _dir){

	if (getSquareFormationActive() && hasSquareFormationAbility()){
		return SF_ACTIVE;
	}


	bool oppositeRotation{_dir == opposite(dir)};

	if (getHasMeleeAttacked() || (!getSkirmish() && getHasRangedAttacked()) || ((getSkirmish() && !oppositeRotation) && getHasAnyAttacked())){
		return NO_ROTATE_AFTER_ATTACK;
	}
	else if ((!getSkirmish() && getHasAnyRotated()) || (getSkirmish() && getHasAnyRotated() && !getHasRangedAttacked())){
		return ALREADY_ROTATED;
	}
	else if (dir == _dir){
		return ALREADY_FACING + UnitTile::dirToString();
	}

	//If it was a full rotation
	if (oppositeRotation){
		if (getSkirmish() && oppositeRotation && getHasRangedAttacked()){
			mov = 2;
		}
		else{
			mov = 0;
		}

		hasFullRotated = true;

		//Due to the rule that cav cannot attack after full rotation, and to simplify matters, I set the
		//attack values to their cap here
		meleeAttacks = getChargesPerTurn();
		rangedAttacks = getShotsPerTurn();
	}
	else{
		hasPartialRotated = true;
	}

	dir = _dir;
	updateStats();

	return SUCCESSFUL_ROTATION + UnitTile::dirToString();
}

std::string Cavalry::meleeAttack(UnitTile* _unit){
	return _unit->meleeAttack(this);
}

std::string Cavalry::meleeAttack(Infantry* inf){

	boost::random::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(world->masterManager.randomEngine)};
	int enemyRoll_int{distribution(world->masterManager.randomEngine)};

	float thisRoll = thisRoll_int;
	float enemyRoll = enemyRoll_int;

	float damageDealt{0};
	float damageReceived{0};

	multRollByModifiers(thisRoll);
	inf->multRollByModifiers(enemyRoll);

	if (hasLancerAbility()){
		if (lancerBonus(inf, thisRoll, lancerBonusReady, damageDealt)){
			mov = 0;
			this->updateStats();
			inf->updateStats();
			meleeAttacks++;

			return attackReport(1, this, inf, thisRoll_int, enemyRoll_int, damageDealt, damageReceived);
		}
	}

	if (abs(thisRoll - enemyRoll) < 0.01){
		damageDealt += 0.5;
		damageReceived += 1;

		inf->takeDamage(this, damageDealt, 1);
		this->takeDamage(inf, damageReceived, 1);
	}
	else if (thisRoll > enemyRoll){
		damageDealt += 4;

		inf->takeDamage(this, damageDealt, 1);

	}
	else if (enemyRoll > thisRoll){
		damageDealt += 1;
		damageReceived += 2;

		inf->takeDamage(this, damageDealt, 1);
		this->takeDamage(inf, damageReceived, 1);

	}

	mov = 0;
	this->updateStats();
	inf->updateStats();
	meleeAttacks++;

	return attackReport(1, this, inf, thisRoll_int, enemyRoll_int, damageDealt, damageReceived);
}

std::string Cavalry::meleeAttack(Cavalry* cav){

	boost::random::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(world->masterManager.randomEngine)};
	int enemyRoll_int{distribution(world->masterManager.randomEngine)};

	float thisRoll = thisRoll_int;
	float enemyRoll = enemyRoll_int;

	float damageDealt{0};
	float damageReceived{0};

	multRollByModifiers(thisRoll);
	cav->multRollByModifiers(enemyRoll);

	if (hasLancerAbility()){
		if (lancerBonus(cav, thisRoll, lancerBonusReady, damageDealt)){
			mov = 0;
			this->updateStats();
			cav->updateStats();
			meleeAttacks++;

			return attackReport(1, this, cav, thisRoll_int, enemyRoll_int, damageDealt, damageReceived);
		}
	}

	if (abs(thisRoll - enemyRoll) < 0.01){
		damageDealt += 1;
		damageReceived += 1;

		this->takeDamage(cav, damageReceived, 1);
		cav->takeDamage(this, damageDealt, 1);
	}
	else{
		//If the difference between rolls is less than 3
		if (abs(thisRoll - enemyRoll) < 3){
			//Player with the highest roll inflicts 1 DMG on the other
			if (thisRoll > enemyRoll){
				damageDealt += 1;
				cav->takeDamage(this, damageDealt, 1);
			}
			else if (enemyRoll > thisRoll){
				damageReceived += 1;
				this->takeDamage(cav, damageReceived, 1);
			}
		}
		//If the difference is greater or equal to 3,
		else{
			if (thisRoll > enemyRoll){
				damageDealt += 2;
				cav->takeDamage(this, damageDealt, 1);
			}
			else if (enemyRoll > thisRoll){
				damageReceived += 2;
				this->takeDamage(cav, damageReceived, 1);
			}
		}
	}

	mov = 0;
	this->updateStats();
	cav->updateStats();
	meleeAttacks++;

	return attackReport(1, this, cav, thisRoll_int, enemyRoll_int, damageDealt, damageReceived);

}

std::string Cavalry::meleeAttack(Artillery* art){

	float damageDealt{0};
	float damageReceived{0};

	boost::random::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(world->masterManager.randomEngine)};

	float thisRoll = thisRoll_int;

	multRollByModifiers(thisRoll);

	if (thisRoll > 3 || abs(thisRoll - 3) < 0.01){
		damageDealt += 3;

		art->takeDamage(this, damageDealt, 1);

	}
	else if (thisRoll < 3){
		damageDealt += 3;
		damageReceived += 3;

		art->takeDamage(this, damageDealt, 1);
		this->takeDamage(art, damageReceived, 1);
	}


	mov = 0;
	this->updateStats();
	art->updateStats();
	meleeAttacks++;

	return attackReport(1, this, art, thisRoll_int, 0, damageDealt, damageReceived);
}

std::string Cavalry::meleeAttack(Mortar* mor){

	float damageDealt{0};
	float damageReceived{0};

	boost::random::uniform_int_distribution<int> distribution(1, 6);

	int thisRoll_int{distribution(world->masterManager.randomEngine)};

	float thisRoll = thisRoll_int;

	multRollByModifiers(thisRoll);

	if (thisRoll > 3 || abs(thisRoll - 3) < 0.01){
		damageDealt += 2;

		mor->takeDamage(this, damageDealt, 1);

	}
	else if (thisRoll < 3){
		damageDealt += 2;
		damageReceived += 0.5;

		mor->takeDamage(this, damageDealt, 1);
		this->takeDamage(mor, damageReceived, 1);
	}


	mov = 0;
	this->updateStats();
	mor->updateStats();
	meleeAttacks++;

	return attackReport(1, this, mor, thisRoll_int, 0, damageDealt, damageReceived);
}
