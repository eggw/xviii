﻿#pragma once

#include "TerrainTile.h"
#include "FontManager.h"

#include <random>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <math.h>

#include "messages.h"

class UnitLoader;

#define MAINTYPEPROPERTIES\
	X("INF", UnitTile::UnitType::INF, Infantry)\
	X("CAV", UnitTile::UnitType::CAV, Cavalry)\
	X("ART", UnitTile::UnitType::ART, Artillery)\
	X("MOR", UnitTile::UnitType::MOR, Mortar)\
	X("GEN", UnitTile::UnitType::GEN, General)\
	X("ARTGUARD", UnitTile::UnitType::ARTGUARD, Infantry)


#define FAMILYTYPEPROPERTIES\
	X("LINF", UnitTile::UnitFamily::LINF_FAMILY)\
	X("INF", UnitTile::UnitFamily::INF_FAMILY)\
	X("HINF", UnitTile::UnitFamily::HINF_FAMILY)\
	X("CAV", UnitTile::UnitFamily::CAV_FAMILY)\
	X("ART", UnitTile::UnitFamily::ART_FAMILY)\

class Player;

class Infantry;
class Cavalry;
class Artillery;
class Mortar;
class General;
class Artguard;

class UnitTile : public Tile
{
public:
	using unitPtr = std::unique_ptr<UnitTile>;

	enum class Direction{ N, E, S, W };

	enum class Modifier{NONE, ADDITIONAL, MULTIPLICATIONAL, ATK, DFND, DISTANCE, FRONT_FLANK, SIDE_FLANK, REAR_FLANK};

	enum class UnitType{INF, CAV, ART, MOR, GEN, ARTGUARD};

	//Each UnitFamily generally has its own combat rules. LINF, however, does not, and for the moment, HINF's
	//is identical to INF.
	enum class UnitFamily{LINF_FAMILY, INF_FAMILY, HINF_FAMILY, CAV_FAMILY, ART_FAMILY};

	//Used for storing modifier information

	struct ModifierReport{

		ModifierReport(Modifier _modType, float _modFloat) : 
		modType{_modType},
		modFloat{_modFloat}
		{
		}

		Modifier modType;
		float modFloat;
	};

	struct RangedAttackRange{
		RangedAttackRange(int _lowerThreshold, int _upperThreshold, float _distModifier, bool _modifierIsDamage, int _lowerDieThreshold, int _upperDieThreshold) :
			lowerThreshold{_lowerThreshold},
			upperThreshold{_upperThreshold},
			distModifier{_distModifier},
			modifierIsDamage{_modifierIsDamage},
			lowerDieThreshold{_lowerDieThreshold},
			upperDieThreshold{_upperDieThreshold}
		{
		}

		//The lower and upper thresholds specify the range at which the accompanying distModifier will 
		//take effect. For example, if:
		//lowerThreshold = 3;
		//upperThreshold = 5;
		//distModifier = 1.5;
		//Then the unit will get a distance modifier of 1.5 when shooting at a distance between 3 and 5 (inclusive)

		//The lower and upper die roll variables only allow damage to be inflicted if the player's roll
		//is between their values; it defaults to 1 and 6

		//The optional modifierIsDamage variable specifies whether the distModifier is a multiplicational damage modifier 
		//or directly damage (1.5d or 1.5 dmg)

		int lowerThreshold;
		int upperThreshold;

		int lowerDieThreshold;
		int upperDieThreshold;

		float distModifier;
		bool modifierIsDamage;
	};

	struct HealingRange{
		HealingRange(int _lower, int _upper, float _healingAmount) :
			lowerThreshold{_lower},
			upperThreshold{_upper},
			healingAmount{_healingAmount}
		{
		}

		//Note that an upper and lower threshold of both 0 means unlimited healing range

		int lowerThreshold;
		int upperThreshold;
		float healingAmount;
	};

	struct FlankModifiers{
		FlankModifiers(UnitTile::UnitType _type) : 
			type{_type}
		{
		}

		UnitTile::UnitType type;
		float front{1};
		float side{1};
		float rear{1};
	};

	struct BonusVSMainType{
		BonusVSMainType(UnitType _mainType, float _modifier, bool _modifierIsAdditional, bool _whenAttacking, bool _whenDefending) :
			mainType{_mainType}, modifier{_modifier}, modifierIsAdditional{_modifierIsAdditional}, whenAttacking{_whenAttacking}, whenDefending{_whenDefending}
		{}

		UnitTile::UnitType mainType;
		float modifier;
		bool modifierIsAdditional;
		bool whenAttacking;
		bool whenDefending;
	};

	struct BonusVSFamilyType{
		BonusVSFamilyType(UnitFamily _familyType, float _modifier, bool _modifierIsAdditional, bool _whenAttacking, bool _whenDefending) :
			familyType{_familyType}, modifier{_modifier}, modifierIsAdditional{_modifierIsAdditional}, whenAttacking{_whenAttacking}, whenDefending{_whenDefending}
		{}

		UnitTile::UnitFamily familyType;
		float modifier;
		bool modifierIsAdditional;
		bool whenAttacking;
		bool whenDefending;
	};

	struct BonusVSName{
		BonusVSName(std::string _name, float _modifier, bool _modifierIsAdditional, bool _whenAttacking, bool _whenDefending) :
			name{_name}, modifier{_modifier}, modifierIsAdditional{_modifierIsAdditional}, whenAttacking{_whenAttacking}, whenDefending{_whenDefending}
		{}

		std::string name;
		float modifier;
		bool modifierIsAdditional;
		bool whenAttacking;
		bool whenDefending;
	};

	UnitTile(UnitLoader& _unitLoader, World& _world, std::mt19937& _mt19937, Player* _belongsToPlayer, TextureManager& _tm, FontManager& _fm, TextureManager::Unit _texture, std::string _name, UnitType _type, UnitFamily _familyType, Direction _dir);
	//Create a virtual destructor, signifying this is an abstract class
	virtual ~UnitTile() = 0;

	//Getters
	//////////////////////////////////////////////////////////////////////////////////////////////////

	//Non-Virtual
	inline TerrainTile* getTerrain() const{ return terrain; };
	inline Player* getPlayer() const{ return player; };
	inline UnitType getUnitType() const{ return unitType; };
	inline UnitFamily getUnitFamilyType() const { return unitFamilyType; };
	inline float gethp() const{ return hp; };
	inline int getMov() const{	return mov;	};
	inline UnitTile::Direction getDir() const{ return dir; };
	inline std::string getName() const{ return name; };

	inline bool getSquareFormationActive(){ return squareFormationActive; };
	inline bool getHasMoved() const{ return hasMoved; };
	inline bool getHasPartialRotated() const{ return hasPartialRotated; };
	inline bool getHasFullRotated() const{ return hasFullRotated; };
	inline bool getHasAnyRotated() const{ return (hasPartialRotated || hasFullRotated); };
	inline bool getHasMeleeAttacked() const{ return hasMeleeAttacked; };
	inline bool getHasRangedAttacked() const{ return hasRangedAttacked; };
	inline bool getHasAnyAttacked() const{ return (hasMeleeAttacked || hasRangedAttacked); };
	inline bool getHasHealed() const{ return hasHealed; };

	bool getMelee() const;
	bool getSkirmish() const;
	bool getFrightening() const;
	bool getHalfRangedDamage() const;
	bool getLancer() const;

	int getMaxRange() const;
	bool canHeal() const;
	bool canRangedAttack() const;

	int getCost() const;
	int getLimit() const;
	int getMaxHp() const;
	int getMaxMov() const;

	float getFlankModifier(UnitType _mainType, Modifier _flank) const;

	//////////////////////////////////////////////////////////////////////////////////////////////////

	//Setters
	//////////////////////////////////////////////////////////////////////////////////////////////////

	inline void sethp(float _hp){ hp = _hp; };
	inline void setMov(float _mov){ mov = _mov; };
	inline void setDir(Direction _dir){ dir = _dir; };
	//inline void setSquareFormationActive(bool _value){ squareFormationActive = _value; };
	inline void setHasMoved(bool _hasMoved){ hasMoved = _hasMoved; };
	inline void setHasPartialRotated(bool _hasPartialRotated){ hasPartialRotated = _hasPartialRotated; };
	inline void setHasFullRotated(bool _hasFullRotated){ hasFullRotated = _hasFullRotated; };
	inline void setHasMeleeAttacked(bool _value){ hasMeleeAttacked = _value; };
	inline void setHasRangedAttacked(bool _value){ hasRangedAttacked = _value; };
	inline void setHasHealed(bool _value){ hasHealed = _value; };

	//////////////////////////////////////////////////////////////////////////////////////////////////

	//The following are applicable only for some children
	inline virtual void setUniqueVariable(bool _uniqueVariable){};
	inline virtual bool getUniqueVariable() const{ return false; };

	//////////////////////////////////////////////////////////////////////////////////////////////////

	//Other - Virtual
	//////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void takeDamage(UnitTile* attacker, float& _dmg, int distance);
	virtual std::string moveTo(TerrainTile* _terrainTile);
	//Called at the end of every turn;
	virtual void reset();
	inline virtual std::string rotate(Direction _dir){ return{}; };

	//This function is unit-specific, and checks for such things as vs. family bonuses/maluses. The bool specifies
	//whether the unit is attacking or defending
	void applyModifiers(UnitTile* _unit, bool _attacking);

	//Needed for double dispatch
	virtual std::string meleeAttack(UnitTile* _unit) = 0;

	inline virtual std::string meleeAttack(Infantry* inf){ return{}; };
	inline virtual std::string meleeAttack(Cavalry* cav){ return{}; };
	inline virtual std::string meleeAttack(Artillery* art){ return{}; };
	inline virtual std::string meleeAttack(Mortar* mor){ return{}; };

	virtual std::string rangedAttack(UnitTile* unit, int distance);

	//Further documented in UnitTile.cpp
	virtual sf::Vector2i distanceFrom(TerrainTile* _terrain, bool& _validMovDirection, bool& _validAttackDirection, bool& _obstructionPresent, bool& _inMovementRange, bool& _inRangedAttackRange, bool canShootOverUnits = false, int coneWidth = 1);

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	//////////////////////////////////////////////////////////////////////////////////////////////////


	//Other - Non Virtual
	//////////////////////////////////////////////////////////////////////////////////////////////////
	void toggleSquareFormationActive();
	std::string heal(UnitTile* _unit);
	std::string beHealed(float num);
	void calculateEffectiveMov();

	//Returns true if dead
	bool removeIfDead();

	UnitTile::Direction opposite(UnitTile::Direction _dir);

	//Spawn is very similar to moveTo, but is only used during the setup phase; it costs no movement 
	void spawn(TerrainTile* terrainTile);

	//This function is in charge of initiating combat by determining distance, flank, etc. and calling either
	//meleeAttack() or rangedAttack()
	std::string attack(UnitTile* _unit);

	bool isHostile(UnitTile* _tile);

    //Manages the position of the sf::Text numbers (hp and movement)
	void updateStats();

	//Overloaded version that only gets the distance and doesn't take in bools
	int distanceFrom(Tile* _tile);

	std::string dirToString();
	std::string modToString(ModifierReport _mod);

	std::string attackReport(int distance, UnitTile* attacker, UnitTile* defender, int attackerRoll, int defenderRoll, float attackerInflicted, float defenderInflicted, bool retreat = false);	

	void multRollByModifiers(float &originalRoll);
	std::string roundFloat(const double x);

	//////////////////////////////////////////////////////////////////////////////////////////////////

	//Data member
	std::vector<ModifierReport> modVector;

protected:
	std::mt19937& mt19937;

	//Pointer to the player the unit belongs to
	Player* player;

	//The unit's current direction (NESW)
	Direction dir;

	//In addition to each player having a flag, each unit itself must have a flag sprite too
	sf::Sprite unitFlag;
	//Pointer to the terrain tile that the unit is on. This is only to be used for units, to indicate
	//where they are easily and efficiently, since a unit must always be on a terrain tile
	TerrainTile* terrain;

	//For (yellow) outlining
	sf::RectangleShape yellowOutline;
	//For (red) outlining
	sf::RectangleShape redOutline;

	//The visual representations of the stats
    sf::Text dirText;
	sf::Text hpText;
	sf::Text movText;

	//This string serves both as a name and as a unique ID to the custom defined unit type
	std::string name;

	UnitType unitType;
	UnitFamily unitFamilyType;

	UnitLoader& unitLoader;

	float hp;
	int mov;

	//Artillery won't make use of this, but others will
	bool squareFormationActive{false};

	bool hasMoved{false};
	bool hasPartialRotated{false};
	bool hasFullRotated{false};
	bool hasMeleeAttacked{false};
	bool hasRangedAttacked{false};
	bool hasHealed{false};
};

