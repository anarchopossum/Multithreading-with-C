#ifndef DUNGEON_SETTINGS_H
#define DUNGEON_SETTINGS_H
#include <signal.h>

//This specifies how many possible characters you can have in your spell buffer. It is a char array. Default: 100
#define SPELL_BUFFER_SIZE (100)

//This is how many rounds the dungeon will play at a minimum. Default: 10
#define NUM_ROUNDS (10)

//This is how long the game will allow the rogue to try to guess the correct number for picking the lock. Default: 4
#define SECONDS_TO_PICK (4)

//This is how many seconds the wizard will have to guess decode the message for the barrier. Default: 2
#define SECONDS_TO_GUESS_BARRIER (2)

//This is how long the barbarian has to attack the monster. Default: 2
#define SECONDS_TO_ATTACK (2)

//This is the tolerance for guessing the rogue's pick. Lower values will be more difficult. Default: 2.5
#define LOCK_THRESHOLD (2.5)

//If true, the barbarian rounds can run. If false, barbarian rounds will not be run. Default: true
#define ALLOW_BARBARIAN true

//If true, the rogue rounds can run. If false, rogue rounds will not be run. Default: true 
#define ALLOW_ROGUE true

//If true, the wizard rounds can run. If false, rogue rounds will not be run. Default: true
#define ALLOW_WIZARD true

//How often the dungeon will check the value of the rogue's pick to see if the lock angle was guessed correctly.
//Lower values will increase the rate of checks, but will also put more messages in the terminal. Default: 10000
#define TIME_BETWEEN_ROGUE_TICKS (10000)

//The maximum angle used for the rogue's picking challenge. Default: 100
#define MAX_PICK_ANGLE (100)

//This is the signal that the dungeon will use to communicate with user processes. Default: SIGUSR1
#define DUNGEON_SIGNAL (SIGUSR1)

//This is the signal that the dungeon will use to tell the processes to use their semaphores. Default: SIGUSR2
#define SEMAPHORE_SIGNAL (SIGUSR2)

//The minimum number of times the barbarian will run the dungeon. Default: 2
#define MIN_BARBARIAN_RUNS (2)

//The minimum number of times the rogue will run the dungeon. Default: 2
#define MIN_ROGUE_RUNS (2)

//The minimum number of times the wizard will run the dungeon. Default: 2
#define MIN_WIZARD_RUNS (2)

//How long the treasure door remains open automatically.The information must be copied from inside, 
//and then the door allowed to close before this amount of time passes. Default: 10
#define TIME_TREASURE_AVAILABLE (10)

//This is how many points you receive for each correct character when getting the treasure at the end. Default: 4
#define POINTS_PER_TREASURE_CHAR (4)

//This is how many points you get for unblocking the semaphores after getting the treasure at the end. Default: 4
#define POINTS_FOR_POSTING_SEMAPHORES (4)

#endif
