/*
* Created: 21.01.2019 19:44:37
* Author : Karl Õmblus
*/


#include "main.h"
#include "rtc.c"


//                               0           1          2           3         4             5            6         7           8             9         NULL
static uint8_t numbrid[] = {0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110, 0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110, 0b00000000};
//      7
//   2     6
//      1
//   3     5
//      4        0


static char ringtone1[] = {
        //     0           1          2           3         4             5            6         7           8             9
        0b10000000, 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b10001000, 0b00000000, 0b00000000, 0b00000000,
        0b10010001, 0b00000000, 0b00000000, 0b10010010, 0b00000000, 0b00000000, 0b00000000, 0b10010010, 0b00000000, 0b00000000,
        0b11001100, 0b11001100, 0b00000000, 0b00000000, 0b11001100, 0b11001100, 0b00000000, 0b00000000, 0b11001100, 0b00000000,
        0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100,
};


static char ringtone3a[] = {
        //     0           1          2           3         4             5            6         7           8             9
        0b10000000, 0b10010000, 0b00000100, 0b00110010, 0b00010000, 0b10010001, 0b00001010, 0b11000000, 0b11111111, 0b11111111,
        0b10010101, 0b10101010, 0b10000000, 0b10010010, 0b00000000, 0b00000000, 0b00000000, 0b10010010, 0b00000000, 0b00000000,
        0b11001100, 0b11001100, 0b00000000, 0b00000000, 0b11001100, 0b11001100, 0b00000000, 0b00000000, 0b11001100, 0b00000000,
        0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100, 0b11001100,
};

static char ringtone3b[] = {
        //     0           1          2           3         4             5            6         7           8             9
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11111111, 0b11111111, 0b11111111,
        0b10000000, 0b00000000, 0b00000000, 0b11111111, 0b11111111, 0b10000000, 0b00000000, 0b11111111, 0b11111111, 0b11111111,
        0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b11111111, 0b11111111, 0b11111111,
        0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b11111111, 0b11111111, 0b11111111,
};

static char ringtone4a[] = {
        //     0           1          2           3         4             5            6         7           8             9
        0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10001000, 0b10000000, 0b10000000, 0b10000000,
        0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000,
        0b11100000, 0b11100000, 0b11100000, 0b11100000, 0b11100000, 0b11100000, 0b11100000, 0b11100000, 0b11110000, 0b11110000,
        0b11110000, 0b11110000, 0b11110000, 0b11110000, 0b11100000, 0b11110000, 0b11110000, 0b11110000, 0b11110000, 0b11110000,
};

static char ringtone4b[] = {
        //     0           1          2           3         4             5            6         7           8             9
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
};


volatile uint8_t mode = 0; // mis reziimis hetkel oleme                                 (volatile, sest main muudab, timer3 kasutab)
volatile uint8_t t_sec = 88; // hetke kellaaeg                                    (pin change interrupt muudab, timer3 kasutab)
volatile uint8_t t_min = 88; // kuni RTC-st pole tulnud näitame ilmselget jama
volatile uint8_t t_hr = 88;

volatile uint8_t aktiivne = 0; // milline number on aktiivne seadistamise reziimis
volatile uint8_t useridle = 0; // kui kasutaja midagi ei näpi, siis läheme kellareziimi tagasi
volatile int8_t vilkumine = 3;     // aktiivse vilgutamise (kiiruse) meelespea


volatile uint8_t alarm1On = 0; // alarmi seaded  (volatile, sest muudame mainis, PCINT0 kontrollib ja timer3 piniseb)
volatile uint8_t alarm1Hr = 0;
volatile uint8_t alarm1Min = 0;
volatile uint8_t alarm2On = 0; // teine alarm tundus ka hea idee
volatile uint8_t alarm2Hr = 0;
volatile uint8_t alarm2Min = 0;
volatile uint8_t alarmRunning = 0;   // meil pole oluline kumb alarm seda teeb
uint16_t mitmesbitt = 0; // peame arvet piiksumise mustri üle (nagu pointer)
volatile uint8_t snoozeminutes = 0;      // automaatvaigistuse paus
uint8_t lastminute = 0;                  // minutite eristamine pausi lugemiseks
uint8_t snoozetimes = 0;                 // mitu korda on venitatud

volatile uint8_t pwm = 0;  // "ekraani" heledus

volatile int8_t timerHr = 0; // taimeri järelejäänud aeg
volatile int8_t timerMin = 0;
volatile int8_t timerSec = 0;
volatile uint8_t timerRunning = 0;
volatile uint8_t stoperHr = 0; // stopperi aeg
volatile uint8_t stoperMin = 0;
volatile uint8_t stoperSec = 0;
volatile uint8_t stoperRunning = 0;
volatile uint8_t stoperHrLap = 0;  // stopperi lap time
volatile uint8_t stoperMinLap = 0;
volatile uint8_t stoperSecLap = 0;

uint8_t s_buffer[8];
uint8_t bt_buffer[6] = {0, 0, 0, 0, 0, 0};
int8_t s_buffer_pos = 0; // esimene vaba positsioon

void buffer_add(uint8_t a) {
    s_buffer[s_buffer_pos] = a;
    s_buffer_pos++;
    if (s_buffer_pos > 7) s_buffer_pos = 0;
}

int buffer_get() {
    s_buffer_pos--;
    if (s_buffer_pos < 0) s_buffer_pos = 7;
    return s_buffer[s_buffer_pos];
}


void serial_send(int a) {
    while (!(UCSR1A & (1 << UDRE1))); // ootame et saame kirjutada
    UDR1 = a;
}


// bad interrupt debug :)
ISR ( INT0_vect  ){serial_send('a');   serial_send('B'); serial_send('\n');}
ISR ( INT1_vect  ){serial_send('b');   serial_send('B'); serial_send('\n');}
ISR ( INT2_vect  ){serial_send('c');   serial_send('B'); serial_send('\n');}
ISR ( INT3_vect  ){serial_send('d');   serial_send('B'); serial_send('\n');}
ISR ( INT6_vect  ){serial_send('e');   serial_send('B'); serial_send('\n');}
ISR ( TIMER1_CAPT_vect  ){serial_send('f');   serial_send('B'); serial_send('\n');}
ISR ( TIMER1_COMPA_vect  ){serial_send('g');   serial_send('B'); serial_send('\n');}
ISR ( TIMER1_COMPB_vect  ){serial_send('h');   serial_send('B'); serial_send('\n');}
ISR ( TIMER1_COMPC_vect  ){serial_send('i');   serial_send('B'); serial_send('\n');}
ISR ( TIMER1_OVF_vect  ){serial_send('j');   serial_send('B'); serial_send('\n');}
ISR ( TIMER0_COMPA_vect  ){serial_send('k');   serial_send('B'); serial_send('\n');}
ISR ( TIMER0_COMPB_vect  ){serial_send('l');   serial_send('B'); serial_send('\n');}
ISR ( TIMER0_OVF_vect  ){serial_send('m');   serial_send('B'); serial_send('\n');}
ISR ( SPI_STC_vect  ){serial_send('n');   serial_send('B'); serial_send('\n');}
ISR ( TIMER3_COMPB_vect  ){serial_send('o');   serial_send('B'); serial_send('\n');}
ISR ( USART1_UDRE_vect  ){serial_send('p');   serial_send('B'); serial_send('\n');}
ISR ( USART1_TX_vect  ){serial_send('q');   serial_send('B'); serial_send('\n');}
ISR ( ANALOG_COMP_vect  ){serial_send('r');   serial_send('B'); serial_send('\n');}
ISR (  ADC_vect ){serial_send('s');   serial_send('B'); serial_send('\n');}
ISR ( EE_READY_vect  ){serial_send('t');   serial_send('B'); serial_send('\n');}
ISR ( TIMER3_CAPT_vect  ){serial_send('u');   serial_send('B'); serial_send('\n');}
ISR (  TWI_vect ){serial_send('v');   serial_send('B'); serial_send('\n');}
ISR ( TIMER3_COMPC_vect  ){serial_send('w');   serial_send('B'); serial_send('\n');}
ISR ( TIMER3_OVF_vect  ){serial_send('x');   serial_send('B'); serial_send('\n');}
ISR (  SPM_READY_vect ){serial_send('y');   serial_send('B'); serial_send('\n');}
ISR ( TIMER4_COMPA_vect  ){serial_send('z');   serial_send('B'); serial_send('\n');}
ISR (  TIMER4_COMPB_vect ){serial_send('1');   serial_send('B'); serial_send('\n');}
ISR ( TIMER4_COMPD_vect  ){serial_send('2');   serial_send('B'); serial_send('\n');}
ISR (  TIMER4_OVF_vect ){serial_send('3');   serial_send('B'); serial_send('\n');}
ISR ( TIMER4_FPF_vect  ){serial_send('4');   serial_send('B'); serial_send('\n');}
ISR ( USB_GEN_vect  ){serial_send('5');   serial_send('B'); serial_send('\n');}
ISR ( USB_COM_vect  ){serial_send('6');   serial_send('B'); serial_send('\n');}
ISR ( WDT_vect  ){serial_send('7');   serial_send('B'); serial_send('\n');}


ISR (USART1_RX_vect){
        int sis=UDR1;

        if (sis=='h') {
            ds3231_set_hour(buffer_get() + buffer_get() * 10);
            serial_send(sis);
            serial_send('O');
            serial_send('K');
            serial_send('\n');
        }
        else if (sis=='m') {
            ds3231_set_minutes(buffer_get() + buffer_get() * 10);
            serial_send(sis);
            serial_send('O');
            serial_send('K');
            serial_send('\n');
        }
        else if (sis=='s') {
            ds3231_set_sec(buffer_get() + buffer_get() * 10);
            serial_send(sis);
            serial_send('O');
            serial_send('K');
            serial_send('\n');
        }
        else if (sis=='A') {
            alarmRunning = buffer_get();
            serial_send(sis);
            serial_send('O');
            serial_send('K');
            serial_send('\n');
        }
        else if (sis=='a') {
            stopAlarm();
            serial_send(sis);
            serial_send('O');
            serial_send('K');
            serial_send('\n');
        }
        else if (sis=='b') {
            bt_buffer[0] = buffer_get();
            bt_buffer[1] = buffer_get();
            bt_buffer[2] = buffer_get();
            bt_buffer[3] = buffer_get();
            bt_buffer[4] = buffer_get();
            bt_buffer[5] = buffer_get();
            mode = 7;
            useridle = 0; // jätame mõneks ajaks ekraanile
            serial_send(sis);
            serial_send('O');
            serial_send('K');
            serial_send('\n');
        }

        else {
            buffer_add(sis - 48);
        }
}


void mode_change_data_save() {
    // uuendame alarmi aega eepromis ainult mode vahetamisel. Ei ole mõtet kontrollida mis modest või kas midagi muudeti, vastasel juhul ei kirjutata niikuinii
    eeprom_update_byte((uint8_t *) 0, alarm1On);
    eeprom_update_byte((uint8_t *) 1, alarm1Min);
    eeprom_update_byte((uint8_t *) 2, alarm1Hr);
    eeprom_update_byte((uint8_t *) 3, alarm2On);
    eeprom_update_byte((uint8_t *) 4, alarm2Min);
    eeprom_update_byte((uint8_t *) 5, alarm2Hr);
    eeprom_update_byte((uint8_t *) 6, pwm);

}

void refresh_screen(int vilguta) {
    //uint8_t sreg_copy = SREG; // ei kutsu hetkel mittekakestusest välja
    //cli(); // muidu on risk, et keset uuendust katkestus kutsub teise uuenduse ja ekraan kirjutatakse sodi täis
    PORTB &= ~(1 << PINB1); // latch/LE low


    //if (vilkumine) PORTD |= (1 << PIND4); // debug led 1
    //if (!vilkumine) PORTD &= ~(1 << PIND4); // debug led 0
    PORTD ^= ~(1 << PIND4); // debug led toggle

    if (vilguta) {
        vilkumine--;                   // loeme alla ja vilgutame 0 juures - siis saame nupu näppimisel kauemaks põlema jätta, jube halb on vilkumise ajal kiiresti kerida
        if (vilkumine < 0) vilkumine = 1;
    }

    //debug
    //serial_send('V');serial_send(vilkumine+48);
    //serial_send('A');serial_send(aktiivne+48);
    //serial_send('\n');


    if (alarmRunning && !vilkumine) { // sõltumata reziimist juures paneme kogu ekraani vilkuma kui alarm on
        writeBits(0);
        writeBits(1 & !!alarm2On);
        writeBits(1 & !!alarm1On);
        writeBits(timerRunning);
        writeBits(stoperRunning);
        writeBits(0);
    } else if (mode == 0) { // tavaline kell
        if (!(alarmRunning && !vilkumine)) {
            writeBits(numbrid[(t_hr / 10) % 10]);
            writeBits(numbrid[t_hr % 10] | (vilkumine & 1 & !!alarm2On));
            writeBits(numbrid[(t_min / 10) % 10] | (~(vilkumine & 1) & !!alarm1On));
            writeBits(numbrid[t_min % 10] | (~(vilkumine & 1) & timerRunning));
            writeBits(numbrid[(t_sec / 10) % 10] | ((vilkumine & 1) & stoperRunning));
            writeBits(numbrid[t_sec % 10]);
        } else {
            kakstyhja(); // vilkumise pime osa - siia ei jõuta, aga jätan selle koodi veel alles, võibolla läheb vaja
            kakstyhja();
            kakstyhja();
        }


    } else if (mode == 1) { // äratuse1 seadistamine
        if (!(aktiivne == 2 && !vilkumine)) {
            writeBits(numbrid[(alarm1Hr / 10) % 10]);
            writeBits(numbrid[alarm1Hr % 10]);
        } else kakstyhja();
        if (!(aktiivne == 1 && !vilkumine)) {
            writeBits(numbrid[(alarm1Min / 10) % 10] | 1); // alarmi seadistamises vasak ülemine täpp põleb
            writeBits(numbrid[alarm1Min % 10]);
        } else {
            writeBits(1);
            writeBits(0);
        }                 // ka siis kui number vilgub
        if (!(aktiivne == 0 && !vilkumine)) {
            writeBits(0);
            writeBits(numbrid[alarm1On]);
        } else kakstyhja();
        if (aktiivne > 2) aktiivne = 0;  // saab muuta kolme seadistust


    } else if (mode == 2) { // äratuse2 seadistamine

        if (!(aktiivne == 2 && !vilkumine)) {
            writeBits(numbrid[(alarm2Hr / 10) % 10]);
            writeBits(numbrid[alarm2Hr % 10] | 1);
        } else {
            writeBits(0);
            writeBits(1);
        }
        if (!(aktiivne == 1 && !vilkumine)) {
            writeBits(numbrid[(alarm2Min / 10) % 10]);
            writeBits(numbrid[alarm2Min % 10]);
        } else kakstyhja();
        if (!(aktiivne == 0 && !vilkumine)) {
            writeBits(0);
            writeBits(numbrid[alarm2On]);
        } else kakstyhja();
        if (aktiivne > 2) aktiivne = 0;

    } else if (mode == 3) { // stopper

        if (aktiivne == 0) {                                    // tavaline stopper
            writeBits(numbrid[(stoperHr / 10) % 10]);
            writeBits(numbrid[stoperHr % 10]);
            writeBits(numbrid[(stoperMin / 10) % 10]);
            writeBits(numbrid[stoperMin % 10]);
            writeBits(numbrid[(stoperSec / 10) % 10] | 1);    // stopperil põleb paremal ülemine punkt
            writeBits(numbrid[stoperSec % 10]);
        } else if (!(aktiivne == 1 && !vilkumine)) {   // stopperi lap time vilgub
            writeBits(numbrid[(stoperHrLap / 10) % 10]);
            writeBits(numbrid[stoperHrLap % 10]);
            writeBits(numbrid[(stoperMinLap / 10) % 10]);
            writeBits(numbrid[stoperMinLap % 10]);
            writeBits(numbrid[(stoperSecLap / 10) % 10] | 1);    // stopperil põleb paremal ülemine punkt
            writeBits(numbrid[stoperSecLap % 10]);
        } else { // pime
            kakstyhja();
            kakstyhja();
            writeBits(1);
            writeBits(0);

        }
        if (aktiivne > 1) aktiivne = 0;

    } else if (mode == 4) { // taimer



        if (!(aktiivne == 3 && !vilkumine)) {
            writeBits(numbrid[(timerHr / 10) % 10]);
            writeBits(numbrid[timerHr % 10]);
        } else {
            writeBits(0);
            writeBits(0);
        }
        if (!(aktiivne == 2 && !vilkumine)) {
            writeBits(numbrid[(timerMin / 10) % 10]);
            writeBits(numbrid[timerMin % 10] | 1);          // taimeri punkt  paremal all
        } else {
            writeBits(0);
            writeBits(1);
        }
        if (!(aktiivne == 1 && !vilkumine)) {
            writeBits(numbrid[(timerSec / 10) % 10]);
            writeBits(numbrid[timerSec % 10]);
        } else {
            writeBits(0);
            writeBits(0);
        }
        //writeBits( numbrid[aktiivne] );
        if (aktiivne > 3) aktiivne = 0; // neli positsiooni




    } else if (mode == 5) { // kella seadistamine

        if (!(aktiivne == 2 && !vilkumine)) {
            writeBits(numbrid[(t_hr / 10) % 10]);
            writeBits(numbrid[t_hr % 10] | 1);
        } else {
            writeBits(1); // jätame kooloni ledid põlema, siis saame aru kus oleme
            writeBits(1);
        }
        if (!(aktiivne == 1 && !vilkumine)) {
            writeBits(numbrid[(t_min / 10) % 10] | 1);
            writeBits(numbrid[t_min % 10] | 1);
        } else {
            writeBits(1);
            writeBits(1);
        }
        if (!(aktiivne == 0 && !vilkumine)) {
            writeBits(numbrid[(t_sec / 10) % 10] | 1);
            writeBits(numbrid[t_sec % 10]);
        } else {
            writeBits(1);
            writeBits(1);
        }
        if (aktiivne > 2) aktiivne = 0; // saab muuta kolme seadistust

    } ///////////////////////////////////////////////////////////////
    else if (mode == 6) { // lisaseaded



        kakstyhja();
        kakstyhja();
        if (!(aktiivne == 0 && !vilkumine)) {
            writeBits(0b00011100);
            writeBits(numbrid[(255 - pwm) / 25 - 1]);
            debug_piuks();
        } else kakstyhja();
        if (aktiivne > 0) aktiivne = 0;  // pole muid seadistamise valikuid hetkel siin reziimis

    } else if (mode == 7) { // BT info kuvamine
        writeBits(numbrid[bt_buffer[5]]);
        writeBits(numbrid[bt_buffer[4]]);
        writeBits(numbrid[bt_buffer[3]]);
        writeBits(numbrid[bt_buffer[2]]);
        writeBits(numbrid[bt_buffer[1]]);
        writeBits(numbrid[bt_buffer[0]]);
    }


    PORTB |= (1 << PINB1); // latch/le high
    //SREG = sreg_copy;
} // refresh_screen

void update_time_from_rtc() {
    uint8_t h, m, s;
    ds3231_hms(&h, &m, &s); // TODO: uurida volatilega ei saanud funktsiooni kutsuda ??
    t_hr = h;
    t_min = m;
    t_sec = s;
}

ISR(PCINT0_vect) { // kellakivi tiksus meile (pool) hertsi edasi


        // sekund muutus
        if (!(PINB&(1<<PINB4))) { // uuendame kella ja tegevusi vaid siis kui sekund on muutunud
            //serial_send(vilkumine+48);serial_send('S');serial_send('\n'); //uus sekund
            update_time_from_rtc();


            // kontrollime kas mõni äratus peaks käima minema    kui mõni äratus läheb, siis juba käiva alarmi ja snooze lülitame välja
            if (t_sec == 0 && t_min == alarm1Min && t_hr == alarm1Hr && alarm1On) alarmRunning = alarm1On;
            if (t_sec == 0 && t_min == alarm2Min && t_hr == alarm2Hr && alarm2On) alarmRunning = alarm2On;

            // tegeleme stopperiga
            if (stoperRunning) {
                stoperSec++;
                if (stoperSec > 59) {
                    stoperSec = 0;
                    stoperMin++;
                }
                if (stoperMin > 59) {
                    stoperMin = 0;
                    stoperHr++;
                }
                if (stoperHr > 99) { // läheme uuele ringile
                    stoperHr = 0;
                }

            }

            // tegeleme timeriga
            if (timerRunning) {
                timerSec--;
                if (timerSec < 0) {
                    timerSec = 59;
                    timerMin--;
                }
                if (timerMin < 0) {
                    timerMin = 59;
                    timerHr--;
                }
                if (timerHr < 0) { // obvious error
                    timerHr = 0;   // set alarm conditions
                    timerMin = 0;
                    timerSec = 0;
                }
                if (timerHr == 0 && timerMin == 0 && timerSec == 0) { // timer alarm
                    //if (mode == 4) { // mulle meeldib kui 0 enne välja kirjutatakse ja siis alles piiksuma hakkab
                    //	vilkumine=4;
                    //	refresh_screen();
                    //}
                    timerRunning = 0;
                    alarmRunning = 4; // 4 on timeri alarmi heli
                }

            }


            //if (!(PINE & (1 << PINE6))) { // meil ei ole toidet     failure.  ei saa peale und enam üles, pin change interrupt pole piisav
            //	debug_piuks();                                       seega backuptoitelt võtame 9ma. Kasutatava aku puhul kui 4 päeva on vool ära -> aku tühi
            //	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
            //	sleep_enable();
            //	sleep_mode();
            //	sleep_disable();
            //}

            refresh_screen(0);

        } // sekund muutus


}


inline void Aon() { PORTC |= (1 << PINC6); }

inline void Aoff() { PORTC &= ~(1 << PINC6); }

inline void Bon() { PORTC |= (1 << PINC7); }

inline void Boff() { PORTC &= ~(1 << PINC7); }

// alarmi piiksumiste jada loomine
void playAlarmBitt() {

    if (alarmRunning == 1) {
        if ((ringtone1[mitmesbitt >> 3]) & (1 << (7 - (mitmesbitt % 8)))) Aon(); else Aoff();
    } else if (alarmRunning == 2) {
        if ((ringtone1[mitmesbitt >> 3]) & (1 << (7 - (mitmesbitt % 8)))) Aon(); else Aoff();
    } else if (alarmRunning == 3) {
        if ((ringtone3a[mitmesbitt >> 3]) & (1 << (7 - (mitmesbitt % 8)))) Aon(); else Aoff();
        if ((ringtone3b[mitmesbitt >> 3]) & (1 << (7 - (mitmesbitt % 8)))) Bon(); else Boff();
    } else { // kas on timeri muster või mingi vea tõttu tundmatu.
        if ((ringtone4a[mitmesbitt >> 3]) & (1 << (7 - (mitmesbitt % 8)))) Aon(); else Aoff();
        if ((ringtone4b[mitmesbitt >> 3]) & (1 << (7 - (mitmesbitt % 8)))) Bon(); else Boff();
    }


    mitmesbitt++;
    if (mitmesbitt > 320) {// snooze läheb peale
        mitmesbitt = 160;    // ei alga enam algusest
        snoozeminutes = 2;
        Aoff();    // seni on vaikus
        Boff();
        snoozetimes++;
        if (snoozetimes > 5) { // lootusetu
            stopAlarm();
        }
    } // paneme snooze peale
}


ISR (TIMER3_COMPA_vect) {
        //debug_piuks();


        useridle++;       // võib end lõhki countida, pole oluline ja ei vaja kontrollimist
        if (useridle > 120 && mode == 4) {
            aktiivne = 0; // timeri idle korral minnakse timeri algasendisse
        }
        else if (useridle > 120 && mode!=3 && mode!=4)  {  // kui user X/6  sekundit midagi ei tee, siis läheb tagasi kellaaja peale  (va. stopper ja timer)
            mode = 0;
            mode_change_data_save(); // siin vaja äratuse data salvestada juhuks kui kasutaja oli selle muutmises
        }




        if (alarmRunning) {
            if (snoozeminutes == 0) playAlarmBitt(); // muudame heli kui  alarm runnib ja snooze ei ole peal
            if (snoozeminutes > 0 && lastminute != t_min) {
                snoozeminutes--;
                lastminute = t_min;
            }
        } // alarmrunning


        if ( t_sec == 0 && t_min == alarm1Min && t_hr == alarm1Hr && alarm1On ) {
            alarmRunning = alarm1On;
            snoozeminutes = 0; // kui snooze oligi peal, siis hakkame uue alarmi korral kohe uuesti häälitsema
        }
        if ( t_sec == 0 && t_min == alarm2Min && t_hr == alarm2Hr && alarm2On ) {
            alarmRunning = alarm2On;
            snoozeminutes = 0;
        }


        refresh_screen(1);


}


inline void kakstyhja() {
    writeBits(0);
    writeBits(0);
}


int main(void) {
    DDRB = (1 << PINB3) | (1 << PINB2) | (1 << PINB6) | (1 << PINB1); // B3=nihke data,  B2=nihke clock,  B1=nihkeregister latch        B6 - LED ON
    DDRC = (1 << PINC6) | (1 << PINC7); // c6=speaker1   c7=speaker2

    DDRD = (1 << PIND6) | (1 << PIND4); // debug output

    PORTC &= ~(1 << PINC6); // speaker vaikseks
    PORTC &= ~(1 << PINC7);
    //PORTB |= (1 << PINB6); // ledid pimedaks   OE HIGH
    PORTB &= ~(1 << PINB6); // ledid põlema   oE LOW


    PORTD &= ~(1 << PIND6); // debug maha
    PORTD &= ~(1 << PIND4); //


    UBRR1 = 12; //  4000000/(16*9600)-1;
    UCSR1B = (1 << TXEN1) | (1 << RXEN1) | (1 << RXCIE1);
    UCSR1C = (0 << USBS1) | (3 << UCSZ10); // 8data, 1stop bit

    ds3231_init();

    // meenutame ka peale restarti, kas ja mis kell äratust soovitakse
    alarm1On = eeprom_read_byte((uint8_t *) 0);
    alarm1Min = eeprom_read_byte((uint8_t *) 1);
    alarm1Hr = eeprom_read_byte((uint8_t *) 2);
    alarm2On = eeprom_read_byte((uint8_t *) 3);
    alarm2Min = eeprom_read_byte((uint8_t *) 4);
    alarm2Hr = eeprom_read_byte((uint8_t *) 5);
    pwm = eeprom_read_byte((uint8_t *) 6);

    if (alarm1On > 3) alarm1On = 0; // ilmselgelt on midagi pahasti (või uus install)
    if (alarm2On > 3) alarm2On = 0;
    if (pwm > 250) pwm = 0;


    // timer 1 - "ekraani" heleduse pwm
    TCCR1A = (1 << COM1B1) | (0 << WGM11) | (1 << WGM10);
    TCCR1B = (1 << CS10) | (1 << WGM12); // div 1
    TCCR1C = 0;
    OCR1B = pwm;   // Set PWM value

    PCMSK0 |= (1 << PCINT4); // enable pin change detection
    PCICR |= (1 << PCIE0);


    // timer 3 - tegeleb kuvamisega
    TCNT3 = 0; // nulli
    OCR3A = F_CPU / 1024 / 2 / 2; // viide X korda sekundis       7812 = 2x sek
    TCCR3A = 0;
    TCCR3B = (1 << CS30) | (1 << CS32) | (1 << WGM32); //CKDIV1024
    TIMSK3 = (1 << OCIE3A) | (1 << TOIE3);


    sei();




    //b5-vasak    b0-paremal esimene     b7-paremal taga   = nupud
    uint32_t loendur;  // loendamiseks, et saada aru kui kaua nuppu all hoitakse
    while (1) {


        loendur = 0;

        if (!(PINB & (1 << PINB5))) { // mode - vasak
            if (alarm_stop_check()) continue; // kui oli alarm, siis rohkem enam midagi ei tee siin.
            aktiivne = 0; // reziimi muutusel läheb aktiivseks alati esimene
            mode++;
            // uuendame (vajadusel) eepromi datat ainult mode vahetamisel. muidu võib liiga ruttu flashi ära rikkuda.
            mode_change_data_save();
            if (mode > 6) mode = 0; // rohkem pole modesid
            while (!(PINB & (1 << PINB5))) {}; // ootame kuni nupp lahti lastakse
            _delay_ms(100); // ei ole vaja modede vahel nii kiirelt ringi kolistada:P
        }


        if (!(PINB & (1 << PINB0))) { // paremal ees - select - vahetab aktiivset seadistamise reziimis
            if (alarm_stop_check()) continue;
            cli(); // ei taha, et katkestus aktiivse ülecountimiseks ära nulliks kui allpool võib veel väheneda
            aktiivne++;

            if (aktiivne == 1 && mode == 3 && stoperRunning) { // stopperil pandi peale lap time
                stoperHrLap = stoperHr;    // katkestused on maas, seega seis saab konsistente
                stoperMinLap = stoperMin;
                stoperSecLap = stoperSec;
            }
            if (aktiivne == 1 && mode == 3 && !stoperRunning) { // stopperi reset
                stoperHr = 0;    // katkestused on maas, seega seis saab konsistente
                stoperMin = 0;
                stoperSec = 0;
                aktiivne = 0; // tagasi stopperi esmaekraanile
            }



            // kui liiga palju suureneb, siis nullitakse ära (eri reziimidel eri piirid)
            while (!(PINB & (1 << PINB0))) {

                if (mode == 4 && !(PINB & (1 << PINB7))) { // selecti all hoides vajutati timeris sisestus = nullimine
                    aktiivne--; // jätame sama numbri aktiivseks, ehk tahab kasutaja samast kohast edasi seadistada
                    timerHr = 0;
                    timerMin = 0;
                    timerSec = 0;
                    timerRunning = 0;
                    sei();
                    while (!(PINB & (1 << PINB7))); // ootame et sisestus lahti lastaks - et see tegevus toimiks vaid ühe korra
                    while (!(PINB & (1 << PINB0))); // ootame et select lahti lastaks - kui siit lahkume, siis ka ülemine while on läbi
                }

                if (mode == 4 && timerRunning) aktiivne = 0; // kui timer töötab, siis niisama select nupp ei tee midagi

                sei();

            } // ootame kuni nupp lahti lastakse
        }    // kollane



        if (!(PINB & (1 << PINB7))) { // paremal taga - sisestus/seadistus
            if (alarm_stop_check()) continue;
            vilkumine = 7; // korraks ei vilgu kui muudeti midagi

            if (mode == 1 && aktiivne == 0) { // alarmi1 seadistamine
                //alarm1On ^= 1;
                alarm1On++;
                if (alarm1On > 3) alarm1On = 0; // hetke on meil 3 erinevat "ringtone"
            } else if (mode == 1 && aktiivne == 1) {
                alarm1Min++;
                if (alarm1Min >= 60) alarm1Min = 0;
            } else if (mode == 1 && aktiivne == 2) {
                alarm1Hr++;
                if (alarm1Hr >= 24) alarm1Hr = 0;
            } else if (mode == 2 && aktiivne == 0) { // alarmi2 seadistamine
                alarm2On++;
                if (alarm2On > 3) alarm2On = 0;
            } else if (mode == 2 && aktiivne == 1) {
                alarm2Min++;
                if (alarm2Min >= 60) alarm2Min = 0;
            } else if (mode == 2 && aktiivne == 2) {
                alarm2Hr++;
                if (alarm2Hr >= 24) alarm2Hr = 0;
            }

                // 3 stopper
            else if (mode == 3) { // stopper  - start/stop
                stoperRunning ^= 1;
            }

                // 4 timer
            else if (mode == 4 && aktiivne == 0) { // timer tavaasendis - start/stop
                timerRunning ^= 1;
            } else if (mode == 4 && aktiivne == 1) { // timeri sekundid
                timerSec++;
                if (timerSec >= 60) timerSec = 0;
            } else if (mode == 4 && aktiivne == 2) { // timeri minutid
                timerMin++;
                if (timerMin >= 60) timerMin = 0;
            } else if (mode == 4 && aktiivne == 3) { // timeri tunnid
                timerHr++;
                if (timerHr >= 99) timerHr = 0;
            } else if (mode == 5 && aktiivne == 0) { // kella seadistamine
                // nullime sekundid
                ds3231_set_sec(0);
                update_time_from_rtc();
            } else if (mode == 5 && aktiivne == 1) {
                uint8_t minut = t_min;
                minut++;
                if (minut >= 60) minut = 0;
                ds3231_set_minutes(minut);
                update_time_from_rtc(); // loeme tagasi
            } else if (mode == 5 && aktiivne == 2) {
                uint8_t hours = t_hr;
                hours++;
                if (hours >= 24) hours = 0;
                ds3231_set_hour(hours);
                update_time_from_rtc();
            } else if (mode == 6 && aktiivne == 0) {  // heleduse seadistamine
                if (pwm <= 200) {
                    pwm += 25;
                } else {
                    pwm = 0;
                }
                OCR1B = pwm;
            }

            //refresh_screen(); // uuendame ekraani kohe ära peale muutmist

            while (!(PINB & (1 << PINB7))) { // ootame kuni nupp lahti lastakse
                loendur++;
                if (loendur > 60000) {
                    break; // enam ei oota
                }

            } // while
        } // nupp - paremal taga

    } // while


} // main


void writeNR(uint8_t num) {
    writeBits(numbrid[num]);
}

// nihutab 8 bitti numbrite nihkeregistrisse
void writeBits(uint8_t bitsToSend) {

    for (int i = 0; i < 8; i++) {
        PORTB &= ~(1 << PINB2); // klokk 0

        if ((bitsToSend & 1) == 1) { // saada 1
            PORTB |= (1 << PINB3);
        } else {
            PORTB &= ~(1 << PINB3); // saada 0
        }
        PORTB |= (1 << PINB2); // klokk 1
        bitsToSend = (bitsToSend >> 1);
    }


}

void debug_piuks() {
    // debug piuks
    PORTC |= (1 << PINC6); // speaker high
    _delay_ms(7);
    PORTC &= ~(1 << PINC6); // speaker low
}

uint8_t alarm_stop_check() {
    useridle = 0; // user vajutas mingit nuppu = aktiivne
    if (alarmRunning) {
        stopAlarm(); // vaigistame alarmi
        while (!(PINB & (1 << PINB0))) {}; // ükskõik mis nupp meid siia tõi, ootame kuni lahti lastakse
        while (!(PINB & (1 << PINB5))) {};
        while (!(PINB & (1 << PINB7))) {};
        return 1; // oli alarmi vaigistamine - ära nupu vajutusele reageeri
    }
    return 0;
}

void stopAlarm() {
    PORTC &= ~(1 << PINC6); // speaker1 low
    PORTC &= ~(1 << PINC7); // speaker1 low
    alarmRunning = 0;
    snoozetimes = 0;
    snoozeminutes = 0;
    mitmesbitt = 0;
}