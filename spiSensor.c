#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "ringBuffer.h"
#include <pigpio.h>

#define SPI_0_CS_PIN 0 // Chip Select pin for SPI
#define SPI_0_BAUD 500000 // SPI baud rate
#define SPI_0_FLAG 3 // SPI flags (0 for default)

int spi_handle; // Global SPI handle
void adxl345_init(){
    spi_handle = spiOpen(SPI_0_CS_PIN, SPI_0_BAUD, SPI_0_FLAG);
    if (spi_handle <0){
        fprintf(stderr, "Spi communication failed\n");
        exit(EXIT_FAILURE);
    }
    uint8_t tx_buffer[2] = {0x2D,0x08}; // Power Control Register
    uint8_t rx_buffer[2] = {0};
    //send first spi message to start spi measurement for this sensor
    spiXfer(spi_handle, (char*)tx_buffer, (char*)rx_buffer, 2);
    printf("Spi communication to set measurement successful: %d\n",rx_buffer[1]);
}

AccelerometerData read_adxl345_spi() {
    uint8_t tx_buffer[7] = {0xF2,0x0,0x0,0x0,0x0,0x0,0x0}; // Power Control Register 0x11110010 (r/w, multibyte, address which is 0x32)
    uint8_t rx_buffer[7] = {0};
    spiXfer(spi_handle, (char*)tx_buffer, (char*)rx_buffer, 7);
    // Process the raw data (rx_buffer[0] is ignored because it arrived during the command byte)
    int16_t x = (rx_buffer[2] << 8) | rx_buffer[1]; // DATAX1 (MSB) and DATAX0 (LSB)
    int16_t y = (rx_buffer[4] << 8) | rx_buffer[3]; // DATAY1 (MSB) and DATAY0 (LSB)
    int16_t z = (rx_buffer[6] << 8) | rx_buffer[5]; // DATAZ1 (MSB) and DATAZ0 (LSB)

    printf("X: %d | Y: %d | Z: %d\n", x, y, z);
    return (AccelerometerData){x, y, z}; // Simulated reading for now
}
// Producer Thread: Simulating your ADXL345 SPI Sensor
void* sensor_spi_thread(void* arg) {
    
    while(1) {
        AccelerometerData simulated_reading = read_adxl345_spi(); 

        pthread_mutex_lock(&buffer.lock);
        // Push to Ring Buffer
        buffer.data[buffer.head].accelData = simulated_reading;
        buffer.head = (buffer.head + 1) % 100;
        pthread_mutex_unlock(&buffer.lock);

        usleep(20000); // Poll at 50Hz (20ms)
    }
}