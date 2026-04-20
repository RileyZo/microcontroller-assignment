#include <reg51.h>

sbit ADD = P1^0;
sbit REDUCE = P1^1;
sbit OFF = P1^2;

unsigned char code Numbers[10] = {
    0x3F,0x06,0x5B,0x4F,0x66,
    0x6D,0x7D,0x07,0x7F,0x6F
};

void delay(unsigned int times) {
    unsigned long ticks = 100 * time;
    while(ticks > 0) ticks--;
}

void main() {
    int speed = 0;
    bit off = 1;
	P1=0x00;

    while (1) {
        if (ADD == 1){
            delay(10);
            if (ADD == 1){
                if (speed < 9) speed++;
                while (ADD == 1);
            }
        }

        if (REDUCE == 1){
            delay(10);
            if (REDUCE == 1){
                if (speed > 0) speed--;
                while (REDUCE == 1);
            }
        }

        if (OFF == 1){
            delay(10);
            if (OFF == 1){
                speed = 0;
                off = !off;
                while (OFF == 1);
            }
        }

		P3 = Numbers[speed];
        
		if (off) P2 = 0x01;
    	else P2 = 0x00;
    }
}
