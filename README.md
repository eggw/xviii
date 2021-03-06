
## **__NOTE: This is very old and amateur code. This project has served its educational purpose for me and I have moved on from it. It only remains here for archival purposes.__**



Inspired by, and continutally developed with, the ideas and input of a friend.

Build Instructions
===================

Required libraries:

SFML 2.4.2: **system**, **window**, **graphics**, and **network** libraries (linked in that order)

Boost (version 1.63 tested): **filesystem**, **random**, and **algorithm** libraries

Compiled with **TDM-GCC (version 4.9.2, 32 bit, SJLJ)**

Notes
======

There is code for random menu wallpapers. My personal collection is nearly 30MB in size, and thus I won't upload it on Github. However, you are free to place your own images in the dir assets/gfx/backgrounds.

Overview
========
	There are four types of actions:


	1) MOVEMENT

	2) PARTIAL ROTATION
			*Left/Right (90 degrees)

	3) FULL ROTATION
			*180 degrees

	4) ATTACK
			*Includes Firing/Charging/Healing


*INF can either MOVE, PARTIALLY ROTATE, or FULLY ROTATE in addition to attacking.

*CAV can either FULLY ROTATE, or PARTIALLY ROTATE in addition to moving. They can FULLY ROTATE after moving, however, but not before. 

*Unless otherwise stated, units may not ROTATE or MOVE at all AFTER attacking.

*INF by default features a square formation mechanic (although this can be removed/defined for other units as well). SF status can be toggled with F; while a unit is in SF, any melee enemy attack will use the modifier of that enemy's SQUAREFORMATIONMODIFIER.


*ART by default features a limber mechanic (although this can be removed/defined for other units as well). Limber status can be toggled with L; while a unit is limbered, it can move, but cannot attack, and vice versa when it is unlimbered. Note that a unit going from limbered to unlimbered needs to wait one turn before it can attack.


*d stands for [6-sided] die; the roll result is multiplied by the modifier, such that a roll of 4 with modifiers of 0.5d and 1d will yield 2 and 4 DMG respectively


*Incremental modifiers always come after multiplicational ones; a Cuirassier 4 rolling against a Dragoon will get a resultant roll of 9 [(2x4) + 1], not 10 [2x(4+1)].


*When a player's GENERAL dies, inflict 2 DMG to all friendly units. Generals may heal a friendly unit for 2 HP once per turn, by default.


Unit Types 
-----------

There are 6 types of MAIN classes:

The "big three":


	INF (Infantry)
	CAV (Cavalry)
	ART (Artillery)


And then the rest:

	GEN (General) - derived from Cavalry, but has a hardcoded limit of 1 per player. 
    A general may heal up to one friendly unit per turn for 2HP. In addition, if 
    allied units are between 21 and 31 tiles away (radius!) from the general, they 
    get only 75% of their maximum movement at the start of the turn. If they are 
    further than 31, they get only 65%. When a General dies, all friendly units 
    will suffer 2 DMG. If a General is absent (either killed or not spawned to begin
    with), all units will start turns with only 50% of their movement points.
    Values are implicitly rounded down where appropriate.


	ARTGUARD (Artillery Guard) - derived from Infantry. Any adjacent artillery 
	tile will be immune to attacks from the enemy until the ARTGUARD is either 
	killed or moves away.


	SAPPER (Sapper/engineer) - derived from Infantry. Is able to construct bridges 
	over water tiles.

Please note that details (HP, MOV, LIMIT, attacking capabilities, etc.) are defined in assets/units. All units must, however, be based on one of these classes.

See the readme in assets/units for more details and further explanation of game mechanics. 

N.B.: There is also currently an unused Mortar class in the code. Do not use this for now, as it is being revised.


Ranged Combat Rules
--------------------


The player rolls a die and the result is multiplied by the unit's dist./dir. modifier, in addition to terrain modifiers (see assets/terrain)

EXAMPLE:

Player 1 has INF shooting at Player 2's CAV from 2 tiles away. Since INF has a modifier of 2d while shooting from this distance, his roll is multiplied by 2. Player 1 rolls 4 and inflicts 8 DMG on Player  2's CAV.


A unit's cone width determines the horizontal range of their capabilities. For example, a unit with a cone width of 5 can shoot both straight ahead, two tiles to the left, and two tiles to the right.


Melee Combat Rules
------------------

Infantry vs Infantry:

	-Roll dice

	-If difference between player 1 and 2 is less than 3, player with highest roll inflicts 1 DMG

	-If both get the same roll, both take 1 DMG

	-Otherwise, inflict 2 DMG


Cavalry vs Cavalry:

	-Roll dice

	-If difference between player 1 and 2 is less than 3, player with highest roll inflicts 1 DMG

	-If both get the same roll, both take 1 DMG

	-Otherwise, inflict 2 DMG

Infantry vs Cavalry:

	-Roll dice

	-Player with higher roll wins

	-If CAV wins, inflict 4 DMG

	-If equal rolls, CAV takes 1 DMG and INF takes 0.5 DMG

	-If INF wins, inflict 2 DMG (and receive 1)

Infantry/Cavalry vs Artillery:

	-Only INF/CAV rolls

	-If INF/CAV rolls 3 or more, inflict 2 DMG. If 2 or less, both INF/CAV and ART take 3 DMG.


Infantry/Cavalry vs Mortar:

	-Only INF/CAV rolls

	-If INF/CAV rolls 3 or more, inflict 2 DMG. If 2 or less, INF/CAV take 0.5 DMG, MOR takes 2 DMG.



In-Game
-------

During the setup phase, each player deploys their units on the game board. Players are limited to the far reaches of the map during this phase (note that this limitation is currently disabled in order to facilitate testing. You may currently spawn units anywhere). In order to deploy a unit, click on its sprite and then place it on the desired tile.

The indicators under the counters represent the unit's current direction, health, and movement points, respectively. A unit with W 5 4 under it would therefore represent a state in which it is facing west, has 5 HP, and 4 movement points.

During the playing phase, players successively command their units by clicking on the desired unit and giving it movement/attack/rotation orders while selected (see below). The currently selected unit is outlined in yellow, while units that have already attacked this turn are outlined in red.

Line of Sight & Weather
------------------------

In addition to movement points and max attacking range, units are limited by their line of sight. 

Each unit has a primary and secondary visual range. Enemy units outside the primary, but within the secondary, are seen only as flags, with their unit type hidden, and their location may be inaccurate. Units outside of the secondary range are completely invisible. 

A unit can neither move to a tile, nor attack an enemy, that has an enemy unit in the way. Friendly units can move past each other; however, whether they can fire "above" units in the way is determined in their .txt.

XVIII also features a dynamic weather system. Every once in a while, the weather changes, and with it, the visual range of all units is affected. The values are as follows (subject to change), with P representing primary and S secondary visual range:

	Light fog: -2P, -2S

	Heavy fog: -4P, -4S

	Light rain: -1P, -1S

	Heavy rain: -2P, -2S


Additionally, both primary and secondary visual ranges are halved during the night hours (**1900 - 5000**).

Every in-game turn represents the passage of 15 minutes.

Keyboard shortcuts & mouse commands
===================================

Menu Phase
-----------

	W/Up/S/Down keys & Return

Nation Selection Phase
----------------------

	Left/A/Right/D keys & Return

Setup & Playing Phases
----------------------

	H - Hide UI

	R - reset zoom level

	WASD - pan view

	dash (-) - zoom out

	equal (=) - zoom in

	Mouse wheel - zoom out/in

Setup Phase
-----------

	LMB - place unit on tile

	RMB - deselect currently selected sprite, or delete unit at tile

Playing Phase
-------------

	LMB - select a unit and issue movement/attack orders

	LCtrl + LMB - attack a terrain tile. This currently has no effect, unless there is a hidden enemy unit on the tile. When 			used by sappers on water tiles, constructs a bridge.

	RMB - deselect currently selected unit/dismiss current message

	L - Toggle limber (if applicable)

	F - Toggle square formation (if applicable)

	Up/down/left/right arrow keys - rotate a unit north, south, east, and west respectively
