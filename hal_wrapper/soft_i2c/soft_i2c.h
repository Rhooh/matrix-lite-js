/*
 * soft_i2c
 *      This is a basic software implementation of I2C protocol
 *      using WiringPi library GPIO.
 *
 *      https://github.com/electronicayciencia/wPi_soft_i2c
 *
 *      Reinoso G.
 *
 *      -----------------------------------------
 *			Including modifications by Frieder G.
 *      -----------------------------------------
 * 			Replaced original use of WiringPi libary for PI-GPIO with
 * 			the according functions from the Matrix-hal libary to use
 *			with the dedicated Matrix-IO GPIO of Matrix-Creator or Matrix-Voice.
 *
 *			Added advanced functions:
 *				asd
 *
 */

#ifndef _SOFT_I2C_H
#define _SOFT_I2C_H

#include <stdint.h>
#include <string>
using namespace std;

#define I2C_READ   1
#define I2C_WRITE  0
#define I2C_ACK    0
#define I2C_NACK   1

#define I2C_DEBUG
// #define I2C_HARD_DEBUG

#define I2C_FREQ 1000//100000    /* I2C clock max frequency 100kHz */
#define WARN     1         /* print warning message via stderr */

typedef struct {
	int scl;
	int sda;
} i2c_t;

// --------------------------------------------------------
// --------------------------------------------------------

// Misc.
string 	i2c_toString	( i2c_t port );

// Basic Matrix-GPIO-access
void 		pinModeMatrix			( int pin, int isOutput);
void 		digitalWriteMatrix( int pin, int targetLevel);
int  		digitalReadMatrix	( int pin );

// Advanced functions
void 		scanForDevices( i2c_t port );
void 		sendText			( i2c_t port , uint8_t addr , string text );

// --------------------------------------------------------
// --------------------------------------------------------

/* Pull: drives the line to level LOW */
void _i2c_pull(int pin);

/* Release: releases the line */
int _i2c_release(int pin);

/* Release_wait: releases the line and wait until it goes up */
void _i2c_release_wait(int pin);

/* Initializes software emulated i2c */
i2c_t i2c_init(int scl, int sda);

/* Start: pull SDA while SCL is up*/
/* Best practice is test the bus before start */
void i2c_start(i2c_t port);

/* Stop: release SDA while SCL is up */
void i2c_stop(i2c_t port);

/* Reset: software reset bus sequence */
void i2c_reset(i2c_t port);

/* Sends 0 or 1:
 * Clock down, send bit, clock up, wait, clock down again */
void i2c_send_bit(i2c_t port, int bit);

/* Reads a bit from sda */
int i2c_read_bit(i2c_t port);

/* Sends 8 bit in a row, MSB first and reads ACK.
 * Returns I2C_ACK if device ack'ed */
int i2c_send_byte(i2c_t port, uint8_t byte);

/* Reads a byte, MSB first */
uint8_t i2c_read_byte(i2c_t port);

#endif
