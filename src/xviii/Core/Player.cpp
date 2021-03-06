#include "xviii/Headers/stdafx.h"
#include "xviii/Core/Player.h"

#include "xviii/Core/UnitLoader.h"
#include "xviii/Core/FactionLoader.h"
#include "xviii/UI/SpawnableUnit.h"

static const sf::View bottomView{sf::View{sf::FloatRect(1183, 4800, xResolution, yResolution)}};
static const sf::View topView{sf::View{sf::FloatRect(1183, -50, xResolution, yResolution)}};
static const sf::View centerView{sf::View{sf::FloatRect(1183, 2900, xResolution, yResolution)}};

Player::Player(MasterManager& _masterManager, World* _world, std::string _factionID, bool _spawnedAtBottom) :
view{},
masterManager(_masterManager),
world(_world),
units{},
general{nullptr},
spawnableUnits{},
factionID{_factionID},
displayName{},
shortDisplayName{},
factionCulture{},
nationColour{sf::Color::White},
nationFlag{},
deploymentPoints{maxDeploymentPoints},
ready{false},
spawnedAtBottom{_spawnedAtBottom}
{
    displayName = masterManager.factionLoader->customFactions.at(factionID).displayName;
    shortDisplayName = masterManager.factionLoader->customFactions.at(factionID).shortDisplayName;
    factionCulture = masterManager.factionLoader->customFactions.at(factionID).factionCulture;

	sf::Vector2i idealDimensions{7, 2};

	for (auto& customClass : masterManager.unitLoader->customClasses){
		for (auto& availableFaction : customClass.second.availableFactions){

			bool validEra{false};

			for (auto& era : customClass.second.eras){
				if (era == world->getEra() || era == World::Era::ALL){
					validEra = true;
					continue;
				}
			}

			if ((availableFaction == factionID || availableFaction == "ALL") && (validEra || world->getEra() == World::Era::ALL)){
				int index = spawnableUnits.size();
				spawnableUnits.emplace_back(this, customClass.second.unitID,
				sf::Vector2i((index % idealDimensions.x) + 1, (index / idealDimensions.x) + 1));
				break;
			}
		}

		for(auto& availableCulture : customClass.second.availableCultures){

            //First, a quick check to see if this faction was already defined in availableFactions (to avoid duplicate units).
            //If it was, then skip this completely. Otherwise, add the unit to spawnableUnits just like above.

            bool duplicateFaction{false};

            for(auto& availableFaction : customClass.second.availableFactions){

                if(availableFaction == factionID){
                    duplicateFaction = true;
                }
            }

            if(!duplicateFaction){
                bool validEra{false};

                for (auto& era : customClass.second.eras){
                    if (era == world->getEra() || era == World::Era::ALL){
                        validEra = true;
                        continue;
                    }
                }

                if ((availableCulture == factionCulture) && (validEra || world->getEra() == World::Era::ALL)){
                    int index = spawnableUnits.size();
                    spawnableUnits.emplace_back(this, customClass.second.unitID,
                    sf::Vector2i((index % idealDimensions.x) + 1, (index / idealDimensions.x) + 1));
                    break;
                }
            }
		}
	}

    std::string flagTextureID = masterManager.factionLoader->customFactions.at(factionID).textureID;
    nationFlag = masterManager.textureManager->getFlagSprite(flagTextureID);

	//DISABLED FOR TESTING PURPOSES
	/*
	if (spawnedAtBottom){
		view = bottomView;
	}
	else{
		view = topView;
	*/

	view = centerView;
}

bool Player::spawnUnit(std::string _unitID, sf::Vector2i _worldCoords){

	UnitTile::unitPtr ptr;
	UnitTile::Direction dir;

	if (spawnedAtBottom){
		//Because coordinates start at [0,0], in order for the bottom spawner to have the same length of spawning
		//as the top spawner does, we need to subtract 1

		//DISABLED FOR TESTING PURPOSES
		/*
		if (cartesianCoords.y <= (world->getDimensions().y - (world->getDimensions().y/8)) - 1){
			return false;
		}
		*/
		dir = UnitTile::Direction::N;
	}
	else{
		//DISABLED FOR TESTING PURPOSES
		/*
		if (cartesianCoords.y >= (world->getDimensions().y / 8)){
			return false;
		}
		*/
		dir = UnitTile::Direction::S;
	}

	UnitTile::UnitType _type = masterManager.unitLoader->customClasses.at(_unitID).unitType;
	UnitTile::UnitFamily _familyType =masterManager.unitLoader->customClasses.at(_unitID).unitFamilyType;

	//Create the appropriate unit
	switch (_type){
		//string, type, class
		#define X(str, type, cl)\
		case(type):\
			ptr = std::move(std::unique_ptr<cl>(new cl(world, this, _unitID, _type, _familyType, dir)));\
			break;
		MAINTYPEPROPERTIES
		#undef X
	}

    //First of all, a hardcoded check to ensure there is no more than 1 general, despite the limit set

    if(ptr->getUnitType() == UnitTile::UnitType::GEN && general != nullptr){
        return false;
    }

	//Check its deployment cost

	int cost = ptr->getCost();
	int limit = ptr->getLimit();


	bool canAfford = deploymentPoints - cost >= 0;
	bool overLimit{false};

	//If the limit is 0 (i.e. limitless), overLimit is always false.
	//Otherwise, count all the current units of the same type
	//and make sure they aren't over the limit.

	if(limit != 0){
		std::string id = ptr->getUnitID();
		int unitsOfType{0};

		for (auto& unit : units){
			if (unit->getUnitID() == id){
				unitsOfType += 1;
			}
		}

		overLimit = (unitsOfType == limit);
	}


	if (canAfford && !overLimit){
		if (world->canBePlacedAtPixelPos(_worldCoords)){
			deploymentPoints -= cost;
			ptr->spawn(world->terrainAtPixelPos(_worldCoords));

			if (_type == UnitTile::UnitType::GEN){
				general = ptr.get();
			}

			units.emplace_back(std::move(ptr));

			return true;
		}
	}

	return false;
}

//For loading from a save game
void Player::loadUnit(std::string _unitID, sf::Vector2i _pos, UnitTile::Direction _dir, float _hp, float _mov, bool _hasMoved, bool _hasPartialRotated, bool _hasFullRotated, int _meleeAttacks, int _rangedAttacks, bool _hasHealed, bool _squareFormationActive, bool _limber, bool _lancerBonusReady){

	UnitTile::UnitType _type = masterManager.unitLoader->customClasses.at(_unitID).unitType;
	UnitTile::UnitFamily _familyType = masterManager.unitLoader->customClasses.at(_unitID).unitFamilyType;

	UnitTile::unitPtr ptr;

	//Create the appropriate unit
	switch (_type){
		//string, type, class
		#define X(str, type, cl)\
			case(type):\
				ptr = std::move(std::unique_ptr<cl>(new cl(world, this, _unitID, _type, _familyType, _dir)));\
				break;
		MAINTYPEPROPERTIES
		#undef X
	}

	ptr->sethp(_hp);
	ptr->setMov(_mov);

	ptr->setHasMoved(_hasMoved);
	ptr->setHasPartialRotated(_hasPartialRotated);
	ptr->setHasFullRotated(_hasFullRotated);
	ptr->setMeleeAttacks(_meleeAttacks);
	ptr->setRangedAttacks(_rangedAttacks);
	ptr->setHasHealed(_hasHealed);
	ptr->setSquareFormationActive(_squareFormationActive);
	ptr->setLimber(_limber);
	ptr->setLancerBonusReady(_lancerBonusReady);


	ptr->spawn(world->terrainAtPixelPos(sf::Vector2i{_pos.x * masterManager.textureManager->getSize().x, _pos.y * masterManager.textureManager->getSize().y}));

	if (_type == UnitTile::UnitType::GEN){
		general = ptr.get();
	}

	units.emplace_back(std::move(ptr));
}

UnitTile::unitPtr Player::removeUnit(sf::Vector2i _worldCoords){
	if (!(_worldCoords.x > 0 && _worldCoords.y > 0
		&&
		_worldCoords.x < world->getDimensionsInPixels().x
		&&
		_worldCoords.y < world->getDimensionsInPixels().y)){

		return nullptr;
	}

	auto found = world->unitAtPixelPos(_worldCoords);

	//If there is a unit at the tile,
	if (found != nullptr){
		if (found->getPlayer() == this){
			//Search units for the unique pointer corresponding to found...
			for (auto& unique_unit : units){
				if (unique_unit.get() == found){
					unique_unit->getTerrain()->resetUnit();

					if (unique_unit->getUnitType() == UnitTile::UnitType::GEN){
						general = nullptr;
					}
					//temporarily move the unit out of unit vector(so that it does not get
					//instantly deleted as it goes out of scope), erase it, then return it
					auto temp = std::move(unique_unit);
					units.erase(std::remove(units.begin(), units.end(), unique_unit), units.end());
					return temp;
				}
			}
		}
	}

	return nullptr;
}

UnitTile::unitPtr Player::removeUnit(UnitTile* _unit){
	if (_unit == nullptr){
		return nullptr;
	}


	//Search units for the unique pointer corresponding to found...
	for (auto& unique_unit : units){
		if (unique_unit.get() == _unit){

			//In some cases, a unit's terrain pointer might be null, such as when
			//a bridge is being toggled to water.

			if(unique_unit->getTerrain() != nullptr){
                unique_unit->getTerrain()->resetUnit();
			}

			if (unique_unit->getUnitType() == UnitTile::UnitType::GEN){
				general = nullptr;
			}
			//temporarily move the unit out of unit vector(so that it does not get
			//instantly deleted as it goes out of scope), erase it, then return it
			auto temp = std::move(unique_unit);
			units.erase(std::remove(units.begin(), units.end(), unique_unit), units.end());
			return temp;
		}
	}


	return nullptr;
}

void Player::drawUnits(sf::RenderTarget &target, sf::RenderStates states) const{
	for (auto& unit : units){
		unit->draw(target, states);
	}
}
