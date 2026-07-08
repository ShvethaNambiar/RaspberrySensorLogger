#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "ringBuffer.h"

// Consumer Thread: File System Logging
void* logging_thread(void* arg) {
    FILE *file = fopen("sensor_log.csv", "a");
    if (!file) {
        // Print error to standard error stream
        perror("Error opening sensor_log.csv");
        // Exit the entire program safely so it doesn't hang
        exit(EXIT_FAILURE); 
    }

    while(1) {
        pthread_mutex_lock(&buffer.lock);
        if (buffer.tail != buffer.head) { 
            PacketData data_to_log = buffer.data[buffer.tail];
            buffer.tail = (buffer.tail + 1) % 100;
            pthread_mutex_unlock(&buffer.lock);

            fprintf(file, "Timestamp: %ld, Device: %s, X: %d, Y: %d, Z: %d\n", \
                data_to_log.timestamp, 
                data_to_log.device == ACCELEROMETER ? "Accelerometer" : data_to_log.device == TEMPERATURE ? "Temperature" : "Distance",
                data_to_log.x, 
                data_to_log.y, 
                data_to_log.z
            );
            fflush(file); 
        } else {
            pthread_mutex_unlock(&buffer.lock);
            usleep(5000); 
        }
    }
    fclose(file);
    return NULL;
}
