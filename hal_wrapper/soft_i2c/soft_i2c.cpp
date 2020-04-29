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
 */
#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include "soft_i2c.h"
#include "matrix_hal/gpio_control.h"
#include "../matrix.h"
#include <iostream>

// Create GPIOControl object
matrix_hal::GPIOControl gpio_control_i2c;

string logIdentifier = "soft_i2c: ";

// Mainly for debugging
string i2c_toString( i2c_t port ) {
	string returnString = "Printing the given i2c_port:\nSCL-Pin = " + to_string(port.scl) +
		+ "\nSDA-Pin = " + to_string(port.sda) + "\n";
	return returnString;
}

// -----------------------------------------------------------------------
// ---------------------- Matrix-GPIO secific functions ------------------
// -----------------------------------------------------------------------
void pinModeMatrix( int pin, int isOutput) {
	gpio_control_i2c.SetMode(pin , isOutput );
}
void digitalWriteMatrix(int pin, int targetLevel){
	gpio_control_i2c.SetGPIOValue(pin , targetLevel );
}
int digitalReadMatrix( int pin ) {
	pinModeMatrix( pin , 0 );
	return (int)gpio_control_i2c.GetGPIOValue( (uint16_t)pin );
}

// -----------------------------------------------------------------------
// -------------------------- Advanced functions -------------------------
// ----------- (TODO: Move advanced functions to js libary) --------------
// -----------------------------------------------------------------------

void scanForDevices( i2c_t port ){
	int addr;
	for (addr = 0; addr < 128; addr++) {
		i2c_start(port);
		if (i2c_send_byte(port, addr << 1 | I2C_READ) == I2C_ACK)
			printf (" * Device found at %0xh\n", addr);
		i2c_stop(port);
	}
}

/*
	Sends the string <text> over i2c-port <port> while adressing the receiver @ <addr>
*/
void sendText( i2c_t port , uint8_t addr , string text ){
	// Start transmission
	i2c_start(port);
	// Check for acknowledge from i2c-device @addr for a write transmission
	if (i2c_send_byte(port, addr << 1 | I2C_WRITE) == I2C_ACK) {
		printf("Receiver-device @addr = %d present and ready\n",addr);
	}
	else {
		printf("Device @ addr=%d not found.",addr);
	}
	// Send all char's of <text> one by one
	cout << logIdentifier << "Sending text via I2C: ";
	for ( size_t i = 0; i < text.length(); i++ ) {
		printf( "%c", text[i] );
		cout << "(";
		cout << i2c_send_byte( port , text[i] );
		cout << ")";
	}
	cout << endl;

	// Stop transmission
	i2c_stop( port );
}

// -----------------------------------------------------------------------
// ------------------- Mostly untouched original functions ---------------
// ------- ( Except for renamings of the new matrix-gpio-functions ) -----
// -----------------------------------------------------------------------

/* Pull: drives the line to level LOW */
void _i2c_pull(int pin) {
	pinModeMatrix(pin, OUTPUT);
	digitalWriteMatrix(pin, LOW);
	delayMicroseconds((1e6/I2C_FREQ)/2);
}

/* Release: releases the line and return line status */
int _i2c_release(int pin) {
	pinModeMatrix(pin, INPUT);
	delayMicroseconds((1e6/I2C_FREQ)/2);
	return digitalReadMatrix(pin);
}

/* In case of clock stretching or busy bus we must wait */
/* But not wait forever though */
void _i2c_release_wait(int pin) {
	int n = 0;

	pinModeMatrix(pin, INPUT);
	delayMicroseconds((1e6/I2C_FREQ)/2);
	while (!digitalReadMatrix(pin)) {
		if (++n >= 50)	{
			if (WARN) fprintf(stderr, "Warning: I2C Bus busy or defective. Pin %d is LOW for 5s.\n", pin);
			return;
		}
		delay(100);
		pinModeMatrix(pin, INPUT);
	}
	delayMicroseconds((1e6/I2C_FREQ)/2);
}

/* Initializes software emulated i2c */
i2c_t i2c_init(int scl, int sda) {
	cout << logIdentifier << "Starting \"i2c_init\"" << endl;
	i2c_t port;
	port.scl = scl;
	port.sda = sda;
	gpio_control_i2c.Setup(&bus);

	pinModeMatrix(scl, INPUT);
	pinModeMatrix(sda, INPUT);
	i2c_reset(port);
	cout << logIdentifier << "\"i2c_init\" done" << endl;
	return port;
}

/* Start: pull SDA while SCL is up*/
/* Best practice is to ensure the bus is not busy before start */
void i2c_start(i2c_t port) {
	cout << logIdentifier << "Starting \"i2c_start\"" << endl;
	if (!_i2c_release(port.sda))
		i2c_reset(port);
    _i2c_release_wait(port.scl);

	_i2c_pull(port.sda);
	_i2c_pull(port.scl);
	cout << logIdentifier << "\"i2c_start\" done" << endl;
}

/* Stop: release SDA while SCL is up */
void i2c_stop(i2c_t port) {
	cout << logIdentifier << "Starting \"i2c_stop\"" << endl;
	_i2c_release_wait(port.scl);
	if (!_i2c_release(port.sda))
		i2c_reset(port);
	cout << logIdentifier << "\"i2c_stop\" done" << endl;
}

/* Reset bus sequence */
void i2c_reset(i2c_t port) {
	cout << logIdentifier << "Starting \"i2c_reset\"" << endl;
	int i;
	int m = 0;

	_i2c_release(port.sda);

	cout << logIdentifier;
	do {
		// Debugging TBR(To Be Removed)
		cout << m << " , ";
		if ( ( m % 10 ) == 0 ) cout << endl << logIdentifier;

		for (i = 0; i < 10; i++) {
			_i2c_pull(port.scl);
			_i2c_release(port.scl);
		}

		if (++m >= 100) {
			if (WARN) fprintf(stderr, "Warning: I2C Bus busy or defective. SDA doesn't go UP after reset.\n");
			return;
		}
		delay(10);
	} while (!digitalReadMatrix(port.sda));

	_i2c_pull(port.scl);
	_i2c_pull(port.sda);

	i2c_stop(port);
	cout << logIdentifier << "\"i2c_reset\" done" << endl;
}

/* Sends 0 or 1:
 * Clock down, send bit, clock up, wait, clock down again
 * In clock stretching, slave holds the clock line down in order
 * to force master to wait before send more data */
void i2c_send_bit(i2c_t port, int bit) {
	if (bit)
		_i2c_release(port.sda);
	else
		_i2c_pull(port.sda);

	_i2c_release_wait(port.scl);
	_i2c_pull(port.scl);

	_i2c_pull(port.sda);
}

/* Reads a bit from sda */
int i2c_read_bit(i2c_t port) {
	int s;

	_i2c_release(port.sda);
	_i2c_release_wait(port.scl);
	s = digitalReadMatrix(port.sda);
	_i2c_pull(port.scl);
	_i2c_pull(port.sda);

	return s;
}

/* Sends 8 bit in a row, MSB first and reads ACK.
 * Returns I2C_ACK if device ack'ed */
int i2c_send_byte(i2c_t port, uint8_t byte) {
	int i;

	for (i = 0; i < 8; i++) {
		i2c_send_bit(port, byte & 0x80);
		byte = byte << 1;
	}

	return i2c_read_bit(port);
}

/* Reads a byte, MSB first */
uint8_t i2c_read_byte(i2c_t port) {
	int byte = 0x00;
	int i;

	for (i=0; i<8; i++)
		byte = (byte << 1) | i2c_read_bit(port);

	return byte;
}
