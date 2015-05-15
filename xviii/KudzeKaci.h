#pragma once

#include "LightInf.h"

class KudzeKaci : public LightInf
{
public:
	KudzeKaci(World& _world, std::mt19937& _mt19937, Player* _belongsToPlayer, TextureManager& tm, FontManager& fm, UnitTile::Direction _dir);

	inline virtual int getMaxHp() const{ return maxhp; };
	inline virtual int getMaxMov() const{ return maxMov; };
	inline virtual int getMaxRange() const{ return maxRange; };

private:
	static const int maxhp{8};
	static const int maxMov{9};
	static const int maxRange{7};
};

