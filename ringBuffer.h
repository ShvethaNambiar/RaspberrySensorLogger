#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <pthread.h>

typedef enum {
    ACCELEROMETER,
    TEMPERATURE,
    DISTANCE
} DeviceType;

typedef struct {
    DeviceType device;
    long timestamp;
    int x;
    int y;
    int z;
} PacketData;

// Shared Ring Buffer Structures
typedef struct {
    PacketData data[100]; 
    int head;
    int tail;
    pthread_mutex_t lock;
} RingBuffer;

extern RingBuffer buffer; // Declare the buffer as external to be shared across files

#endif // RINGBUFFER_H