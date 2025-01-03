#pragma once

#include <types.h>

enum eEnemyType
{
	Generic,
	Explosive,
	Juggernaut,
	Animal,
	Zombie
};

struct Enemy
{
	Ped ped = 0;
	bool timer = false;
	bool exploded = false;
	bool playedDeathSound = false;
	bool fiery = false;
	Vector3 deadCoords{};
	int timeOfDeath = 0;
	int lastHealth = 0;
	int lastDialog = 0;
	eEnemyType enemyType = eEnemyType::Generic;
};