#include <fcntl.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"



// Can you please review my reading assignments 5 and 6 I turned them in on time
// and also attempted all the questions. sorry I am very Paranoid and think this
// is the only way to get in contact with you. thank you



int main(int args, char *argv[])
{
    //shm_unlink Removes the shared Memory on execute
    shm_unlink(dungeon_shm_name);
    /* shm_open creates or opens existing shared mem Object.
     * O_Create creates the share memory if it doesn't exist
     * O_EXCL throw an error if it already exists.
     * O_RDWR gives R/W permissions
     * 600 is chmod permissions (R/W to owner/user) -- (Switched to 666)
    */
    int shr_mem = shm_open(dungeon_shm_name, O_CREAT | O_EXCL | O_RDWR, 0666);

    // if the d_mem isn't created correctly, it will throw an error.
    if (shr_mem < 0){perror("\n/DungeonMem creation error?\n"); return 0;}

    // Makes sure that the file is within a specific length.
    ftruncate(shr_mem, 3*sizeof(int));

    //struct Dungeon* mapped = (struct Dungeon*)mmap(0, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shr_mem, 0);
    struct Dungeon Dun;

    // PID for characters.
    int par_ID = getpid();
    int ready = 0;

	// from here the process gets forked and execute other class processes, being (Barbarian, Wizard and Rogue)
    int barb =fork();
    if (barb == 0) execv("./barbarian",argv);
    int wiz =fork();
    if (wiz == 0) execv("./wizard",argv);
    int rog =fork();
    if (rog == 0) execv("./rogue",argv);

	// if game has the correct parent ID run Dungeon
    if(par_ID == getpid()){
        sleep(1);
		// main program that uses each of these processes.
        RunDungeon(wiz,rog,barb);
		// kills all the processes after RunDungeon since the other processes can't kill themselves.
        kill(barb,SIGKILL);
        kill(wiz, SIGKILL);
        kill(rog,SIGKILL);
    }
    return 0;
}
