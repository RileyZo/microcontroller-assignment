#include <reg51.h>
#include <intrins.h>

sbit C1 = P2^4;
sbit C2 = P2^5;
sbit C3 = P2^6;
sbit C4 = P2^7;

unsigned int input_count = 0;
unsigned char sending = 0;

void delay(unsigned int time) {
    unsigned long ticks = 100 * time;
    while(ticks-- > 0);
}

unsigned int get_input(unsigned int row) {
    if (C1 == 0) return row * 4 + 0;
    if (C2 == 0) return row * 4 + 1;
    if (C3 == 0) return row * 4 + 2;
    if (C4 == 0) return row * 4 + 3;
    return 16;
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
    unsigned int row = 0;
    unsigned int key = 16;
    unsigned int prev_key = 16;
    P1 = 0x00;
    
    uart_init();
    
    while(1) {
        row = row % 4;
        P2 = _crol_(~0x01, row);
        key = get_input(row);
        
        if (key != 16 && key != prev_key) {
            delay(10);
            if (key == get_input(row)) {
                uart_send(key);
                while(!C1 || !C2 || !C3 || !C4);
                prev_key = 16;
            }
        }
        
        if (key == 16) {
            prev_key = 16;
        }
        
        row++;
        delay(10);
    }
}

void uart_isr() interrupt 4 {
    if (RI) {
        RI = 0;
        input_count = SBUF;
        P1 = input_count;
    }
}