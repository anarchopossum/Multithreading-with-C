/*
 * The Wizard is probably the second class that you should make. The Wizard works as follows:
 When the Wizard receives a signal (defined in dungeon_settings.h), the Wizard reads the Caesar Cypher placed in the
 Barrier's spell field. The Wizard then decodes the Caesar Cypher, using the first character as the key, and copies the
 decoded message into the Wizard's spell field. The Dungeon will wait an amount of time defined in dungeon_settings.h
 as SECONDS_TO_GUESS_BARRIER for the decoding process to complete. If the Wizard's spell field matches the decoded
 message after the Dungeon has finished waiting, then this will count as success.
 */
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctype.h>
#include "dungeon_info.h"
#define IPC_RESULT_ERROR (-1)


typedef struct string string;

// Global Variables that we use in this program
struct Dungeon* mapped;
int shr_mem;
sem_t *lever;

// holdr() uses a semaphore as a manner to hold the Lever.
// this also needs to be closed for this assignment but we added a few seconds for the Rogue
// to do their thing and get out of the dungeon.
static void holdr(){
    lever = sem_open(dungeon_lever_two, O_CREAT, 0666, 1);
    sleep(5);
    lever = sem_close(dungeon_lever_two);
}

static void signalr(int alpine){
    // String declaration: this will be the imported value from memory
    char *spell = mapped->barrier.spell; // this will hold the input for shared memory
    char *ans = mapped->wizard.spell; // this will be the output for the shared memory
    char key_char= spell[0]; // the first letter is the key for the system.
    int  key_val = spell[0] % 26; // key_val gets the char number value in the standard alphabet.
    int temp; // this variable will be used for the algorithm


    memset(ans, 0, sizeof ans); // clears the Memory from mapped wizard spell
	
    for (int i = 1; i < strlen (spell); ++i) {
        if (isalpha(spell[i])) {
// We can use characters instead of numbers to simplify the project. No need to convert to int and back to char
            // We add the 'A' to get the num value up into the appropriate section in Ascii.
            if (spell[i] >= 'A' && spell[i] <= 'Z') {
                temp = spell[i] - 'A';
				// the value cannot be less then the Ascii value 'A', if so increment it by 'A' Ascii value. and do the math algo.
                if (temp - key_val >= 0) { temp= (temp - key_val) % 26 + 'A'; }
                else { temp = (temp - key_val) % 26 + 26 + 'A'; }
			// this case we use 'a' to get the num values for lowercase values in Ascii.
            } else if (spell[i] >= 'a' && spell[i] <= 'z') {
                temp = spell[i] - 'a';
				// the value cannot be less then the Ascii value 'a', if so increment it by 'a' Ascii value. and do the math algo.
                if (temp - key_val >= 0) { temp= (temp - key_val) % 26 + 'a'; }
                else { temp = (temp - key_val) % 26 + 26 + 'a'; }
            }
			// adds the decrypted char into the answer array
            strncat(ans,&temp,1);
        }else{
			// adds the non alphabetical char into the answer array.
            strncat(ans,&spell[i],1);
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
    sigaction2.sa_handler = &holdr;
    sigaction(SIGUSR2,&sigaction2,NULL);
    // infinite loop to keep the process alive until terminated by other means such as the parent process.
    while(1){}
    return 0;
}

