/* PROMPT:
 * The Barbarian is the first character class that you should probably make. The Barbarian works as follows:
 * When the Barbarian receives a signal (defined in dungeon_settings.h), the Barbarian copies the integer in the enemy's
 * health field into the attack field. Use dungeon_info.h to see how the Dungeon struct is set up. The Dungeon will then
 * wait an amount of time defined in dungeon_settings.h as SECONDS_TO_ATTACK. If the integer in attack matches the
 * integer in health, then this will count as success.
 */
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "dungeon_info.h"
#define IPC_RESULT_ERROR (-1)

struct Dungeon* mapped;
int shr_mem;
sem_t *lever;


// holdr() uses a semaphore as a manner to hold the Lever.
// this also needs to be closed for this assignment but we added a few seconds for the Rogue
// to do their thing and get out of the dungeon.
static void holdr(){
    lever = sem_open(dungeon_lever_one, O_CREAT, 0666, 1);
    sleep(5);
    lever = sem_close(dungeon_lever_one);
}

static void signalr(int alpine){
    // write the enemy health to the barbarian attack int on the shared Memory.
    mapped->barbarian.attack = mapped->enemy.health;
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
    sigaction2.sa_handler = &holdr;
    sigaction(SIGUSR2,&sigaction2,NULL);
    // infinite loop to keep the process alive until terminated by other means such as the parent process.
    while(1){}
    return 0;
}
