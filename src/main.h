#pragma once

#define F_CPU 2000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "avr/interrupt.h"
#include <avr/power.h>
#include <avr/sleep.h>

void writeBits(uint8_t bitsToSend); // saadame numbri nihkeregistrisse
void writeNR(uint8_t num); // saadame ühekohalise numbri ekraanile
void kakstyhja();
void debug_piuks();   // väike piik debugiks
uint8_t alarm_stop_check(); // kontrollib kas nupuvajutus peaks alarmi vaigitama tavategevuse asemel
void stopAlarm();