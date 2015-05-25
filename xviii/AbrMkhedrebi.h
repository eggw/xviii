#pragma once

#include "Dragoon.h"

class AbrMkhedrebi : public Dragoon
{
public:
	AbrMkhedrebi(World& _world, std::mt19937& _mt19937, Player* _belongsToPlayer, TextureManager& tm, FontManager& fm, UnitTile::Direction _dir);

	//Skirmisher
	inline virtual bool canSkirmish(){ return true; };

	inline virtual int getMaxHp() const{ return maxhp;};
	inline virtual int getMaxMov() const{ return maxMov;};

private:
	static const int maxhp{8};
	static const int maxMov{12};
};

