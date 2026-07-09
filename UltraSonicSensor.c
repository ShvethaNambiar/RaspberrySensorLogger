#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <pigpio.h>
#include <time.h>
#include "ringBuffer.h"

#define HC_TRIGGER_PIN 27 // Physical 13 is BCM 27
#define HC_ECHO_PIN 22    // Physical 15 is BCM 22

// Global variables for interrupt timing
volatile uint32_t start_tick = 0;
volatile uint32_t end_tick = 0;
volatile int new_reading = 0;
volatile int start_set = 0;

// Callback function triggered automatically on Echo pin state changes
void echo_cb(int gpio, int level, uint32_t tick) {
    if (level == 1) {
        start_tick = tick;
        start_set = 1;
    } else if (level == 0) {
        if (!start_set) return; // ignore falling edge without a valid rising edge
        // Ignore anything under 150 microseconds (electrical cross-talk noise)
        uint32_t diff = tick - start_tick;
        if (diff > 150) {
            end_tick = tick;
            new_reading = 1;
        }
        start_set = 0;
    }
}

void ultrasonic_init(){
    gpioSetMode(HC_TRIGGER_PIN, PI_OUTPUT);
    gpioSetMode(HC_ECHO_PIN, PI_INPUT);
    gpioSetPullUpDown(HC_ECHO_PIN, PI_PUD_DOWN);
    gpioSetAlertFunc(HC_ECHO_PIN, echo_cb);
}

PacketData read_ultrasonic() {
    float distance = -1.0; // Error value
    start_tick = 0;
    end_tick = 0;
    new_reading = 0; // clear any previous reading before triggering
    gpioWrite(HC_TRIGGER_PIN, PI_HIGH);
    gpioDelay(10); // 10 microseconds
    gpioWrite(HC_TRIGGER_PIN, PI_LOW);
    // Timeout mechanism: Wait up to 30ms for the interrupt callback to finish
    int timeout = 30; 
    while (!new_reading && timeout > 0) {
        gpioDelay(1000); // Sleep 1ms
        timeout--;
    }
    if (new_reading) {
        uint32_t travel_time = end_tick - start_tick;
        distance = travel_time * 0.0343 / 2.0;
        printf("Ultrasonic Distance: %f cm\n", distance);
        new_reading = 0; // consumed
    } else {
        printf("Ultrasonic Read Timeout %d\n",timeout);
    }
    return (PacketData){DISTANCE, time(NULL), distance,0,0};  
}

void *ultrasonic_sensor_thread(void *arg){
    while(1){
        PacketData local_buffer = read_ultrasonic();

        pthread_mutex_lock(&buffer.lock);
        buffer.data[buffer.head] = local_buffer;
        buffer.head = (buffer.head + 1) % 100;
        pthread_mutex_unlock(&buffer.lock);

        usleep(60000); // Poll at  (60ms) 
    }
}