#pragma once

#include "xviii/Terrain/TerrainTile.h"

class PBridge : public TerrainTile
{
public:
	PBridge(TerrainLoader& _terrainLoader, World& _world, TextureManager& tm, sf::Vector2f _pos);

    inline void setHp(int _hp){hp = _hp;};
    inline int getHp(){return hp;};

    virtual void takeDamage(int dmg);

    //By default, the bridge spawns with its VER (vertical) version. You can use this to flip it.
    void flip(Orientation _or);

    Orientation getOrientation(){return orientation;};

    virtual std::string callTerrainAttack(UnitTile* unit, int distance);

    bool northernConnection{false};
    bool southernConnection{false};
    bool easternConnection{false};
    bool westernConnection{false};

private:
    int hp;
    Orientation orientation;
};

