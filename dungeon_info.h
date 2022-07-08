#ifndef DUNGEON_INFO_H
#define DUNGEON_INFO_H
#include <stdbool.h>
#include <unistd.h>
#include "dungeon_settings.h"

//This is the name we will use for our shared memory.
const char* dungeon_shm_name = "/DungeonMem";

//These are the names for the levers when getting the treasure at the end.
const char* dungeon_lever_one = "/LeverOne";
const char* dungeon_lever_two = "/LeverTwo";


struct Barbarian{
	int attack;
};
struct Rogue{
	float pick;
};
struct Wizard{
	char spell[SPELL_BUFFER_SIZE];
};
struct Barrier{
	char spell[SPELL_BUFFER_SIZE + 1];
};
struct Enemy{
	int health;
};
struct Trap{
	char direction;
	bool locked;
};
struct Dungeon{
	bool running;
	pid_t dungeonPID;
	struct Barbarian barbarian;
	struct Rogue rogue;
	struct Wizard wizard;
	struct Barrier barrier;
	struct Enemy enemy;
	struct Trap trap;
	char treasure[4];
	char spoils[4];
};

//Call this method to begin running the dungeon. Valid pid's must be passed for it to work.
void RunDungeon(pid_t wizard, pid_t rogue, pid_t barbarian);
#endif
