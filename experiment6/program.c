#include <reg51.h>

unsigned char code Number[] = {
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
    0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71 
};

unsigned char code Position[8] = {
    0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F
};

enum MODE {
    OFF = 0,
    ON = 1,
    START = 2,
    PAUSE = 3
};

unsigned int mode_code = 0;
unsigned long distance = 1234;
unsigned char buffer[8] = {0};

void delay(unsigned int time) {
    unsigned int ticks = 10 * time;
    while(ticks-- > 0);
}

void on_signal_triggered() interrupt 0 {
    if (mode_code == START) {
        distance += 4;
    }
}

void on_button_pressed() interrupt 2 {
    mode_code = (mode_code + 1) % 4;
    if (mode_code == ON) {
        distance = 0;
    }
}

void update_buffer() {
    unsigned char i = 0;
    unsigned long temp = distance;
    unsigned char digit;

    switch(mode_code) {
        case OFF:
            for (i = 0; i < 8; i++) {
                buffer[i] = 0x00;
            }
            break;
        case ON:
            for (i = 0; i < 8; i++) {
                buffer[i] = Number[0];
            }
            break;
        default:
            for (i = 0; i < 8; i++) {
                unsigned long weight = 1;
                unsigned char j;
                for (j = 0; j < (7 - i); j++) {
                    weight *= 10;
                }
                digit = (unsigned char)((temp / weight) % 10);
                buffer[i] = Number[digit];
            }
            break;
    }
}

void display() {
    unsigned char i;

    if (mode_code == OFF) {
        P2 = 0xFF;
        P1 = 0x00;
        return;
    }

    for (i = 0; i < 8; i++) {
        P2 = Position[i];
        P1 = buffer[i];
        delay(30);
    }
}

void main() {
    EA = 1;
    EX0 = 1;
    EX1 = 1;
    IT0 = 1;
    IT1 = 1;
    PX0 = 1;
    PX1 = 0;

    while(1) {
        update_buffer();
        display();
    }
}