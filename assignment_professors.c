#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>  

#define PROFESSOR_COUNT 5

#define STATE_EMPTY 0
#define STATE_LEFT_CHOPSTICK 1
#define STATE_BOTH_CHOPSTICK 2

#define LEFT_DIRECTION -1
#define RIGHT_DIRECTION 1

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int chopstick_owners[5] = { -1, -1, -1, -1, -1 };

const char* professor_names[] = {
    "Tanenbaum",
    "Bos",
    "Lamport",
    "Stallings",
    "Silberschatz"
};

typedef struct {
    short id;
    short state;
} prof_struct;

bool pick_up_chopstick(int prof_id, int direction){
    pthread_mutex_lock(&lock);

    int chopstick_id = prof_id + direction;
    if(chopstick_id < 0)
        chopstick_id = 4;
    else if(chopstick_id > 4)
        chopstick_id = 0;

    bool success = false;
    if(chopstick_owners[chopstick_id] == -1){
        chopstick_owners[chopstick_id] = prof_id;
        success = true;
    }

    pthread_mutex_unlock(&lock);
    return success;
}

void return_left_chopstick(int prof_id){
    int chopstick_id = prof_id + LEFT_DIRECTION;
    if(chopstick_id < 0)
        chopstick_id = 4;
    else if(chopstick_id > 4)
        chopstick_id = 0;
    chopstick_owners[chopstick_id] = -1;
}

void return_right_chopstick(int prof_id){
    int chopstick_id = prof_id + RIGHT_DIRECTION;
    if(chopstick_id < 0)
        chopstick_id = 4;
    else if(chopstick_id > 4)
        chopstick_id = 0;
    chopstick_owners[chopstick_id] = -1;
}

void return_chopsticks(int prof_id){
    return_left_chopstick(prof_id);
    return_right_chopstick(prof_id);
}

void* professor(void* args){
    prof_struct* pargs = (prof_struct*)args;

    int id = pargs->id;
    int state = STATE_EMPTY;
    const char* name = professor_names[id];
    printf("Hi from %s\n", name);

    for(;;)
    {
        switch(state){
            case STATE_EMPTY:
                sleep(rand() % 4 + 2); // 2 - 10 sec
                
                if(pick_up_chopstick(id, LEFT_DIRECTION)){
                    state = STATE_LEFT_CHOPSTICK;
                    printf("%s: thinking -> got left chopstick\n", name);
                } else {
                    printf("%s: thinking -> did not get left chopstick\n", name);
                }
                break;
            case STATE_LEFT_CHOPSTICK:
                sleep(rand() % 1 + 1);; // 1 - 3 sec
                if(pick_up_chopstick(id, RIGHT_DIRECTION)){
                    state = STATE_BOTH_CHOPSTICK;
                    printf("%s: thinking -> got right chopstick\n", name);
                } else {
                    printf("%s: thinking -> did not get right chopstick\n", name);
                    return_left_chopstick(id);
                    state = STATE_EMPTY;
                }
                break;
            case STATE_BOTH_CHOPSTICK:
                sleep(rand() % 3 + 3);; // 10 - 20 sec
                state = STATE_EMPTY;
                return_chopsticks(id);
                printf("%s: Ate -> Returned all chopsticks\n", name);
                break;
            default:
                printf("Big error oof");
                exit(1);
                break;
        }

    }
    printf("Bye from %s\n", name);
}

int main(void){
    prof_struct* prof_args;
    pthread_t* threads;
    pthread_mutex_init(&lock, 0);
    
    threads = malloc( PROFESSOR_COUNT * sizeof(pthread_t) );
    prof_args = malloc( PROFESSOR_COUNT * sizeof(prof_struct) );

    for(int i = 0; i < PROFESSOR_COUNT; i++){
		prof_args[i].id = i;
		prof_args[i].state = STATE_EMPTY;
		pthread_create(&(threads[i]), // our handle for the child
			NULL, // attributes of the child
			professor, // the function it should run
			(void*)&prof_args[i]); // args to that function

    }

    for(int i = 0; i < PROFESSOR_COUNT; i++)
    {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&lock);
    return 0;
}