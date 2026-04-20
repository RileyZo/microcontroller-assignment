#include <reg51.h>
#include <intrins.h>

unsigned char led = 0x01;

void delay(unsigned int time) reentrant {
    unsigned long ticks = 500 * time;
    while(ticks-- > 0);
}

void on_button1_pressed() interrupt 2 {
    unsigned int i = 0;
    while(i++ < 5) {
        P2 = i % 2 == 0 ? ~0x55 : ~0xAA;
        delay(10);
    } 
}

void on_button2_pressed() interrupt 0 {
    unsigned int i = 0;
    unsigned char state = 0x80;
    while(i++ < 7) {
        P2 = ~state;  
        state = _cror_(state, 1);
        delay(10);
    }
}

void main(void) {
    EA = 1;

    EX0 = 1;
    EX1 = 1;

    IT0 = 1;
    IT1 = 1;
    
    PX0 = 1; 
    PX1 = 0;

    led = 0x01;
    P2 = ~led;

    while(1) {
        led = _crol_(led, 1);
        P2 = ~led; 
        delay(10);
    }
}