#include <reg51.h>

void delay() {
    unsigned long ticks = 5000;
    while (ticks > 0) ticks--;
}

void main() {
    unsigned char state = 0x01;
    unsigned time = 0;
    while (time < 2) {
        while(state != state << 8) {
            P2 = state;
            state = state << 1;
            delay();
        }
        state = 0x01;
        time++;
    }

    state = (char)(1 + 4 + 16 + 64);
    time = 0;
    while (time < 2) {
        P2 = state;
        delay();
        P2 = ~state;
        delay();
        time++;
    }

    state = 0x01 << 7;
    time = 0;
    while (time < 2) {
        while(state != 0x00) {
            P2 = state;
            state = state >> 1;
            delay();
        }
        state = 0x01 << 7;
        time++;
    }

    state = 0x00;
    time = 0;
    while (time < 2) {
        P2 = ~state;
        delay();
        P2 = state;
        delay();
        time++;
    }

    while(1) {}
}