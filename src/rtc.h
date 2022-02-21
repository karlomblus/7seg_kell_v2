#pragma once


void ds3231_init(void);
void ds3231_write_clock_register(uint8_t reg, uint8_t value);

void twi_init(void);
void twi_start(void);
void twi_stop(void);
void twi_send(uint8_t data);
uint8_t twi_receive(uint8_t ack_val);

