#include <stdio.h>
#include <pthread.h>
#include <pigpio.h>
#include <stdlib.h>
#include "AccelSensor.h"
#include "UltraSonicSensor.h"
#include "logger.h"

void initialize_main(){
    if (gpioInitialise() < 0) {
        // Initialization failed
        fprintf(stderr, "Pigpio initialization failed\n");
        exit(EXIT_FAILURE);
    }
    //send first adxl spi message to start measurement
    adxl345_init();
    ultrasonic_init();
}
int main() {
    initialize_main();
    
    pthread_t t1, t2, t3;
    
    // Spin up the threads
    pthread_create(&t1, NULL, accel_spi_thread, NULL);
    pthread_create(&t2, NULL, ultrasonic_sensor_thread, NULL);
    pthread_create(&t3, NULL, logging_thread, NULL);
    
    // Let them run infinitely
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    
    return 0;
}