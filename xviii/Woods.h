#pragma once

#include "TerrainTile.h"

class Woods : public TerrainTile
{
public:
	Woods(World* _world, TextureManager& tm, sf::Vector2f _pos);
};
