#include "xviii/Headers/stdafx.h"
#include "xviii/Core/Tile.h"

#include "xviii/Core/World.h"

Tile::Tile(World* _world, TextureManager::Terrain type) :
world(_world),
sprite{world->masterManager.textureManager->getSprite(type)},
highlighted{false}
{
}

Tile::Tile(World* _world, std::string textureID) :
world(_world),
sprite{_world->masterManager.textureManager->getUnitSprite(textureID)},
highlighted{false}
{
}

Tile::~Tile(){
}

void Tile::setHighlighted(bool _value){
	highlighted = _value;
}

void Tile::setColor(sf::Color _color){
    sprite.setColor(_color);
}

sf::Vector2i Tile::getCartesianPos() const{
	//N.B.
	//In the interest of keeping this function const, I used this
	return world->cartesianPosAtIndex(world->indexAtPixelPos(sf::Vector2i{left(), top()}));
	//Instead of this:
	//Tile* ptr = this;
	//return world->cartesianPosAtIndex(world->indexAtPixelPos(world->indexAtTile(this)));
}

sf::Vector2f Tile::getPixelPos() const{
	return sprite.getPosition();
}

sf::Vector2f Tile::getPixelPosCenter() const{
    return{sprite.getPosition().x + (sprite.getLocalBounds().width / 2), sprite.getPosition().y + (sprite.getLocalBounds().height / 2)};
}

int Tile::top() const{
	return sprite.getPosition().y;
}

int Tile::left() const{
	return sprite.getPosition().x;
}

int Tile::bottom() const{
	return sprite.getPosition().y + sprite.getLocalBounds().height;
}

int Tile::right() const {
	return sprite.getPosition().x + sprite.getLocalBounds().width;
}

