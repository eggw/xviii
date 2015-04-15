#pragma once

#include "LightCav.h"


class Deli : public LightCav
{
public:
	Deli(World& _world, std::mt19937& _mt19937, Player* _belongsToPlayer, TextureManager& tm, FontManager& fm, UnitTile::Direction _dir);

	virtual int getMaxHp() const;
	virtual int getMaxMov() const;
	virtual int getMaxRange() const;

	virtual void preAttack(UnitTile* unit, bool attacking);

private:
	static const int maxhp{9};
	static const int maxMov{16};
	static const int maxRange{0};
};

