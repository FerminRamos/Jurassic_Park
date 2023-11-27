#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>  // Used for sleep()
#include "random437.h"
#include <time.h>
#include <sys/times.h>
#include <getopt.h>
#include <semaphore.h>

int CARNUM;           // N
int MAXPERCAR;        // M
int LOAD_TIME = 7;    // How long car takes to load passengers (sec)
int RIDE_TIME = 53;   // How long car takes to do ride (sec)
int MAXWAITPEOPLE = 800;
void timeInterval(int duration);
void* carOperating();

sem_t sem;

//typedef struct Car{
//    int id;
//    int maxCapacity;
//    int currentCapacity;
//}Car;

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
 * @param args
 * @return
 */
void* startRide(void* args){
    // Load Time
    sleep(0);  // TODO: Replace w/ LOAD_TIME

    // Ride Time
    int* id = (int*) args;
    for(int i = 0; i < 5; i++){  // TODO: Replace w/ RIDE_TIME
        printf("Thread_%d Ride Timer = %d\n", *id, i);
        sleep(1);
    }
    printf("Thread_%d Finished Ride.\n", *id);
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

    int duration = 10 * 60;  // Total "duration" in a 10-hr day

    /**
     * @brief List of Arrivals/Waiting/Rejects
     * for each minute interval
     * 600 minutes in total.
     */
    int arrivals[duration];
    int rejects[duration];
    int waiting[duration];

    int total_arrival = -1;
    int total_reject = -1;
    int total_waiting = -1;


    // Car Rides
    pthread_t carThreads[MAXPERCAR];
    int carIDs[MAXPERCAR];

    for(int i = 0; i < MAXPERCAR; i++){
        carIDs[i] = i;                                                // Assign ID to car
        pthread_create(&carThreads[i], NULL, startRide, &carIDs[i]);  // Do Ride.
    }

    for(int i = 0; i < MAXPERCAR; i++){
        pthread_join(carThreads[i], NULL);  // Blocks Main
    }


    timeInterval(duration);
    return 0;
}