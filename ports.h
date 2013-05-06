/************************************************
 * ports.h
 *
 * Jason Gassel
 ************************************************/

#ifndef PORTS_H
#define PORTS_H

sbit blockReadIndicator = P1^3;
sbit nCS = P1^4;

sbit switch1 = P2^0;
sbit switch2 = P2^1;
sbit switch3 = P2^2;
sbit switch4 = P2^3;
sbit redLED = P2^4;
sbit yellowLED = P2^5;
sbit amberLED = P2^6;
sbit greenLED = P2^7;

sbit mp3_enable = P1^3;
sbit mp3_twi_scl = P3^4;
sbit mp3_twi_sda = P3^5;
sbit mp3_reset = P3^6;
sbit mp3_data_req = P3^7;

#endif //PORTS_H
