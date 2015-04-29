#pragma once

#include "Cuirassier.h"

class Kapikulu : public Cuirassier
{
public:
	Kapikulu(World& _world, std::mt19937& _mt19937, Player* _belongsToPlayer, TextureManager& tm, FontManager& fm, UnitTile::Direction _dir);

	inline virtual int getMaxHp() const{ return maxhp; };
	inline virtual int getMaxMov() const{ return maxMov; };
	inline virtual int getMaxRange() const{ return maxRange; };

	//Kapikulu's reset() is overloaded due to the additional attackBonus variable
	virtual void reset();

	//Overloaded for Kapikulu's bonus:
	//During their FIRST melee roll, if the roll is 6(without any modifier), they inflict 4 Damage to INF units (any type), or 3 Damage to Cav 
	virtual std::string meleeAttack(Infantry* inf);
	virtual std::string meleeAttack(FootGuard* foot);
	virtual std::string meleeAttack(Cavalry* cav);

	//Override Cuirassier's preAttack() with blank function
	virtual void preAttack(UnitTile* unit, bool attacking){};

	//These functions will really be used for the attackBonusReady bool
	virtual bool getHasHealed() const{ return attackBonusReady; };
	virtual void setHasHealed(bool _value){ attackBonusReady = _value; };

private:
	//A special Kapikulu bonus; they have to rest for a turn to recharge this, so that they can proc the above mentioned bonus
	bool attackBonusReady{true};

	static const int maxhp{16};
	static const int maxMov{9};
	static const int maxRange{0};
};

