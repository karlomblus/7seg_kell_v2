#include "main.h"
#include "rtc.h"


// http://ww1.microchip.com/downloads/en/devicedoc/atmel-7766-8-bit-avr-atmega16u4-32u4_datasheet.pdf
void twi_init(void) {
    TWSR = 0x00;       // TWI Status Register          lk 252
    TWBR = 0x0F;       // TWI Bit Rate Register
    TWCR = (1 << TWEN); // TWI Control Register
}

void twi_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Send START condition                (lk 235  p1)
    while (!(TWCR & (1 << TWINT)));        // ootan kuni TWINT->1, seni spinnin
}

void twi_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // Transmit STOP condition     (lk 236  p7)
}

void twi_send(uint8_t data) {
    TWDR = data;                    // Load DATA into TWDR Register.     (lk 236 p5)
    TWCR = (1 << TWINT) | (1 << TWEN);  // Clear TWINT bit in TWCR to start	transmission of data
    while (!(TWCR & (1 << TWINT)));   // Wait for TWINT Flag set. This	indicates that the DATA has been	transmitted, and ACK/NACK has	been received.
}

uint8_t twi_receive(uint8_t ack_val) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (ack_val << TWEA);  // 0 NACK   1 ACK
    while (!(TWCR & (1 << TWINT)));                      //
    return TWDR;
}


void ds3231_write_clock_register(uint8_t reg, uint8_t value) {
    uint8_t sreg_copy = SREG;
    cli();
    twi_start();
    twi_send(0xD0); // initial address of ds3231+0 to write next byte
    twi_send(reg); // select location
    twi_send(value); // update second to zero
    twi_stop();
    SREG = sreg_copy;
}

void ds3231_init(void) {
    twi_init();
    ds3231_write_clock_register(0x0E, 0x00); // int 1 hz
}

void ds3231_set_minutes(uint8_t value) {
    uint8_t tvalue = value / 10;
    value = value % 10;
    value |= (tvalue << 4);
    ds3231_write_clock_register(0x01, value);
}

void ds3231_set_hour(uint8_t value) {
    uint8_t tvalue = value / 10;
    value = value % 10;
    value |= (tvalue << 4);
    value &= 0b00111111; // upper bits anyway 0  (bit 7=0  == 24 hr mode)
    ds3231_write_clock_register(0x02, value);

}

void ds3231_set_sec(uint8_t value) {
    uint8_t tvalue = value / 10;
    value = value % 10;
    value |= (tvalue << 4);
    ds3231_write_clock_register(0x00, value);
}

void ds3231_hms(uint8_t *h, uint8_t *m, uint8_t *s) {
    uint8_t sreg_copy = SREG;
    cli();
    twi_start();
    twi_send(0xD0); // ask writing
    twi_send(0x00); // write what to read
    twi_start();
    twi_send(0xD1);// read
    uint8_t val = twi_receive(1);
    val &= 0b01111111;
    *s = (val & 0b00001111) + 10 * ((val & 0b11110000) >> 4);  // vasakud 10 bitti nihutan paremale ja korrutan 10-ga, tulemuse liidan paremale neljale bitile
    val = twi_receive(1);
    *m = (val & 0b00001111) + 10 * ((val & 0b11110000) >> 4);
    val = twi_receive(0);
    twi_stop();
    *h = (val & 0b00001111) + 10 * ((val & 0b00110000) >> 4); // eeldame, et 24h reziimis
    SREG = sreg_copy;
}


