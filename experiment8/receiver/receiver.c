#include <reg51.h>
#include <intrins.h>

unsigned char code Characters[] = {
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
    0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71
};

unsigned char Buffer[8] = { 16, 16, 16, 16, 16, 16, 16, 16 };
unsigned char received_count = 0;

void delay(unsigned int time) {
    unsigned long ticks = 100 * time;
    while(ticks-- > 0);
}

void update_display() {
    unsigned int index = 0;
    unsigned int key = 0;
    while(index < 8) {
        key = Buffer[index];
        P1 = _crol_(~0x01, index);
        P2 = key < 16 ? Characters[key] : 0x00;
        index++;
        delay(1);
    }
}

void push_buffer(unsigned int key) {
    int index = 6;
    while(index >= 0) {
        Buffer[index + 1] = Buffer[index];
        index--;
    }
    Buffer[0] = key;
    received_count++;
}

void send(unsigned char dat) {
    SBUF = dat;
    while(!TI);
    TI = 0;
}

void main() {
    SCON = 0x50;
    TMOD = 0x20;
    TH1 = 0xFD;
    TL1 = 0xFD;
    TR1 = 1; 
    ES = 1;
    EA = 1;

    while (1) {
        update_display();
    }
}

void on_received() interrupt 4 {
    unsigned char key;
    if (RI) {
        RI = 0;
        key = SBUF;
        if (key < 16) {
            push_buffer(key);
            send(received_count);
        }
    }
}