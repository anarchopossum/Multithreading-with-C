#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <math.h>
#include "dungeon_info.h"
#define IPC_RESULT_ERROR (-1)

struct Dungeon* mapped;
int shr_mem;


static void grabr(){
	// This grabs the treasure from the dungeon after the levers are pulled.
	// it ignores the garbage being null characters.
	// basically there are 4 slots in the spoils if there is a desired value add it.
    char *t = mapped->treasure;
    for (int i = 0; i < sizeof(t); ++i) {
        if(t[i] == '\0') --i;
        else strncat(mapped->spoils, &t[i],1);
    }
}
static void signalr(int alpine){
	// made guess equal pick but honestly it's useless.
    int guess = mapped->rogue.pick;
    guess = 100;

    while(mapped->trap.direction != '-') {	// while the trap is not open
        if (mapped->trap.direction != 't') { // checks the test char to make sure the formula works.
            if (mapped->trap.direction == 'u') { // if the value is higher than the current guess
                guess = guess + (guess / 2);
            } else if (mapped->trap.direction == 'd') { // if the value is lower than the current guess
                guess = guess / 2;
            }
            mapped->trap.direction = 't'; // forgot this, without it the program won't work
            mapped->rogue.pick = guess; // makes the guess equal the pick to see if it works.
            if (guess == 0) guess=100; // if the guess gets stuck on 0, reset the value.
        }
    }
}

int main(int args, char *argv[]){
    // Uses the sharemem with W/R access similar to the game.c
    shr_mem = shm_open(dungeon_shm_name, O_EXCL | O_RDWR, 0666);
    mapped = (struct Dungeon*) mmap(0,sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shr_mem, 0);

    // sigaction to handle the function handlr via sigusr1 as a trigger.
    struct sigaction sigaction1 = {0};
    sigaction1.sa_flags = SA_RESTART;
    sigaction1.sa_handler = &signalr;
    sigaction(SIGUSR1,&sigaction1,NULL);

	// This second sigaction is for holding the lever, we use sigusr2 for this
    struct sigaction sigaction2 = {0};
    sigaction2.sa_flags = SA_RESTART;
    sigaction2.sa_handler = &grabr;
    sigaction(SIGUSR2,&sigaction2,NULL);
    // infinite loop to keep the process alive until terminated by other means such as the parent process.
    while(1){}
    return 0;
}

