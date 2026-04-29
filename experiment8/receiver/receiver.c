#include <reg51.h>
#include <intrins.h>

unsigned char code Characters[] = {
    0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,
    0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E
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
        P1 = _crol_(0x01, index);
        P2 = key < 16 ? Characters[key] : 0xff;
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

void uart_init() {
    SCON = 0x50;
    TMOD &= 0x0F;
    TMOD |= 0x20;
    TH1 = 0xFD;     // 波特率9600 (11.0592MHz)
    TL1 = 0xFD;
    TR1 = 1;
    ES = 1;
    EA = 1;
}

void uart_send(unsigned char dat) {
    SBUF = dat;
    while(!TI);
    TI = 0;
}

void main() {
    uart_init();
    
    while (1) {
        update_display();
    }
}

void uart_isr() interrupt 4 {
    unsigned char key;
    if (RI) {
        RI = 0;
        key = SBUF;
        if (key < 16) {
            push_buffer(key);
            uart_send(received_count);
        }
    }
}