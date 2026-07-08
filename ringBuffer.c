#include "ringBuffer.h"

RingBuffer buffer = { .head = 0, .tail = 0, .lock = PTHREAD_MUTEX_INITIALIZER };