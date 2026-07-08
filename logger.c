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
            DeviceTypes data_to_log = buffer.data[buffer.tail];
            buffer.tail = (buffer.tail + 1) % 100;
            pthread_mutex_unlock(&buffer.lock);

            fprintf(file, "Accelerometer Data: %d, %d, %d, Temperature Data: %.2f, Distance Data: %.2f\n" \
                data_to_log.accelData.x, data_to_log.accelData.y, data_to_log.accelData.z,\
                data_to_log.tempData.temperature, \
                data_to_log.distanceData.distance
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
