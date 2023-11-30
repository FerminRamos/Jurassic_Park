/**
 * List of changes that I made to your code (that I remember):
 *   1. Moved "duration" to be global variable "DURATION" - Want rides to stop when the current time >= DURATION
 *      (aka 7 pm or 600 minutes). So, we need this value in a bunch of threads / functions.
 *   2. In main, where you initialize the values of "DURATION", I moved it down a few lines just bc it looks nicer lol.
 *   3. Added temp values to "CARNUM" and "MAXPERCAR", just bc I hate compiling all the time.
 *
 * What I coded so far:
 *   1. Car Threads.
 *      I made 'N' number of car threads that run forever. They stop when the current time >= DURATION (aka 7 pm). If
 *      you look at the function, I put a brief outline of what we need to do to finish the logic for it.
 *      ** What we need - a waiting line + a mutex lock for the waiting line + some simple logic to pop out X amount
 *      of people from waiting line.
 *   2. Arrival Module.
 *      The arrival module also runs forever. It stops when the current time >= DURATION (aka 7 pm). The function
 *      also has a brief outline of what we need to do to finish the logic for it. It can already call
 *      poissonRandom() to get a random amount of people.
 *      ** What we need - A waiting line + logic to accept / deny people based on waiting line size + logic to
 *      insert people into waiting line + logic create Person structs (if person is accepted).
 *   3. ALSO PLEASE CHANGE HOW I CALCULATE THE TIME. Even if we use a GLOBAL_MINUTE, remove it from
 *      Arrival Module, plz. Leaving it in there doesn't take into account the time it took to do other things
 *      (like making Person structs), it'll actually take more than 1 second. This violates what I was
 *      trying to do, which is that 1 IRL minute = 1 computer second. Thank you!
 *
 * Questions:
 *   1. Do we actually need to keep track of HH:MM:SS? or can we just use a simple int (GLOBAL_MINUTE)? And then
 *      convert to HH:MM:SS.
 *      - I'm saying this because people ONLY arrive into the park every 1 minute.
 *
 *   2. How do you want to keep track of the time? You're the expert on this. I'll just follow along.
 *
 *   3. Technically, we can have 1 Minute IRL = 1 second (or less) in computer time. So if we make it less
 *      than 1 second, what would happen to our Arrival Module if it's still accepting / making Person structs
 *      and the next group arrives? Bc groups arrive to the arrival module every 1 IRL minute. Do we overwrite
 *      the previous group size? Do we "append" (but we'll need some sort of structure for this)? Idk...
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>  // Used for booleans
#include <unistd.h>   // Used for sleep()
#include "random437.h"
#include <time.h>
#include <sys/times.h>
#include <getopt.h>
#include <semaphore.h>

int CARNUM = 1;          // N  TODO: remove default testing val
int MAXPERCAR = 7;       // M  TODO: remove default testing val
int LOAD_TIME = 1;       // How long car takes to load passengers (sec) TODO: replace value
int RIDE_TIME = 3;       // How long car takes to do ride (sec) TODO: replace value
int MAXWAITPEOPLE = 800;
int GLOBAL_MINUTE = 0;
int DURATION;
void timeInterval(int duration);

sem_t sem;
pthread_mutex_t timeMutex;

/**
 * Person struct used to calculate how long a person was waiting in line.
 */
typedef struct Person {
    int timeIn;
    int timeOut;
}Person;


int arrivalRule(int iter) {
    int meanArrival = 0;
    if (iter < 120) {
        meanArrival = 25;
    } else if (iter < 300) {
        meanArrival = 45;
    } else if (iter < 420) {
        meanArrival = 35;
    } else {
        meanArrival = 25;
    }
    meanArrival = poissonRandom(meanArrival);
    return meanArrival;
}

void timeInterval(int duration)
{
    // Set the start time 9:00am
    struct tm start_time = {0};
    start_time.tm_hour = 9;
    start_time.tm_min = 0;
    start_time.tm_sec = 0;

    // Convert start time to time_t
    time_t current_time = mktime(&start_time);

    for (int i = 0; i < duration; ++i)
    {
        struct tm *time_info = localtime(&current_time);

        // Print in HH:MM:SS format
        printf("XXX Arrival, YYY Rejects, ZZZ Waiting, at %02d:%02d:%02d\n", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);

        // Increment current time by 1 minute
        current_time += 60;
    }
}

/**
 * Thread function for a Car ride.
 * It does 2 things:
 *   1. Removes people from waiting line (NOTE ~ we don't need to store people in car
 *      thread, just pop them out of the waiting line)
 *   2. Sleeps until ride is done.
 * @param args car id.
 * @return NULL.
 */
void *startRide(void* args){
    int* id = (int*) args;

    while(true){  // Rides stop @ 7 pm.
        pthread_mutex_lock(&timeMutex);
        int currentTime = GLOBAL_MINUTE;
        pthread_mutex_unlock(&timeMutex);

        if (currentTime >= DURATION) {
            break;  // Exit the loop when the park closes
        }

        // TODO: Add pthread_mutex_lock() to get lock on waiting line passengers

        // TODO: We have lock on waiting line, now subtract -7 (or less) from waiting line.

        // Load Time
        printf("  => Thread_%d Loading Passengers.\n", *id);
        sleep(LOAD_TIME);

        // TODO: Add pthread_mutex_unlock() to release lock on waiting line

        // Ride Time
        printf("  => Thread_%d Passengers Loaded -> Starting Ride.\n", *id);
        sleep(RIDE_TIME);
        printf("  => Thread_%d Finished Ride.\n", *id);
    }
    return NULL;
}


/**
 * Arrival Module is responsible for 4 things:
 *   1. Letting people inside the park (calling poissonRandom())
 *   2. Accepting / Rejecting people for waiting line.
 *   3. Giving "Person struct" to people who were accepted into waiting line
 *   4. Updating Global Time (TODO: Don't update global time in here...)
 * @return
 */
void *arrivalModule(){
    while(true){
        pthread_mutex_lock(&timeMutex);
        int currentTime = GLOBAL_MINUTE;
        pthread_mutex_unlock(&timeMutex);

        if (currentTime >= DURATION) {
            printf("\n@@ PARK CLOSED. @@\n");
            break;  // Exit the loop when the park closes
        }

        // TODO: Open front gates to get potential customers
        printf("<Current Time: %d>\n", currentTime);
        int groupSize = poissonRandom(45);  // TODO: Remember to change meanArrival based on the time...(do that later tho)
        printf("{Arr_Mod} NEW GROUP ARRIVED OF SIZE %d!\n", groupSize);


        // TODO: Acquire waiting line mutex

        // TODO: Accept or Deny people (create Person struct for each accepted person)

        // TODO: Release waiting line mutex


        // TODO: Sleep for 1 "Minute" (or system equivalent) bc people ONLY arrive every 1 minute.
        sleep(1);

        // TODO: Update Global Time
        pthread_mutex_lock(&timeMutex);
        GLOBAL_MINUTE++;
        pthread_mutex_unlock(&timeMutex);
    }
    return NULL;
}


int main(int argc, char **argv){
    int opt = 0;

    // Parse Args
    while ((opt = getopt(argc, argv, "N:M:")) != -1)
    {
        switch (opt)
        {
            case 'N':
                CARNUM = atoi(optarg);
                printf("CARNUM is %d\n", CARNUM);
                break;
            case 'M':
                MAXPERCAR = atoi(optarg);
                printf("MAXPERCAR is %d\n", MAXPERCAR);
                break;
        }
    }

    /**
     * @brief List of Arrivals/Waiting/Rejects
     * for each minute interval
     * 600 minutes in total.
     */
//    DURATION = 10 * 60;  // Total "DURATION" in a 10-hr day
    DURATION = 8;  // temp just for testing :)
    int arrivals[DURATION];
    int rejects[DURATION];
    int waiting[DURATION];

    int total_arrival = -1;
    int total_reject = -1;
    int total_waiting = -1;


    // Global current minute
    pthread_mutex_init(&timeMutex, NULL);

    // Arrival Module
    pthread_t arrivalMod;
    pthread_create(&arrivalMod, NULL, arrivalModule, NULL);

    // Car Rides
    pthread_t carThreads[MAXPERCAR];
    int carIDs[MAXPERCAR];

    for(int i = 0; i < CARNUM; i++){
        printf("New car thread created! id = %d\n", i);
        carIDs[i] = i;                                                // Assign ID to car
        pthread_create(&carThreads[i], NULL, startRide, &carIDs[i]);  // Do Ride.
    }


    // ALL Thread Joins placed here...
    pthread_join(arrivalMod, NULL);         // Arrival Module

    for(int i = 0; i < MAXPERCAR; i++){     // Car Rides
        pthread_join(carThreads[i], NULL);
    }


    timeInterval(DURATION);
    return 0;
}