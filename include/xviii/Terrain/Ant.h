#pragma once

#include "xviii/Core/World.h"

/*
This is the "generic ant" - it generates random clusters of certain tiles, and for
this reason is best suited for MUD, URBAN, WOODS, and MEADOW. It can be used for
WATER to generate small lakes as well.
*/

class TerrainLoader;

class Ant
{
public:
	Ant(TerrainLoader& _terrainLoader, World& _world, TerrainTile::TerrainType _type, int _lifetime);
	virtual void crawl();
protected:
	virtual void increment(int dir, sf::Vector2i& cartesianCoords);

    TerrainLoader& terrainLoader;
	World* world;
	TerrainTile::TerrainType type;
	int lifetime;
	int currentIndex;
};

