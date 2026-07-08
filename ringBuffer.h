#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <pthread.h>

typedef struct {
    int x;
    int y;
    int z;
} AccelerometerData;

typedef struct {
    float temperature;
} TemperatureData;

typedef struct {
    float distance;
} DistanceData;

typedef struct {
    AccelerometerData accelData;
    TemperatureData tempData;
    DistanceData distanceData;
} DeviceTypes;

// Shared Ring Buffer Structures
typedef struct {
    DeviceTypes data[100]; 
    int head;
    int tail;
    pthread_mutex_t lock;
} RingBuffer;

extern RingBuffer buffer; // Declare the buffer as external to be shared across files

#endif // RINGBUFFER_H