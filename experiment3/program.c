#include <reg51.h>

void delay(unsigned int time) {
    unsigned long ticks = 50 * time;
    while (ticks > 0) ticks--;
}

sbit SWITCH = P1^0;

unsigned char code Character[] = {
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
    0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71 
};

void display(unsigned int index, unsigned char character) {
    P2 = ~(0x01 << index);
    P3 = character;
}

void main() {
    unsigned int now_character_index = 0;
    unsigned int display_time = 0;
    while (1) {
        unsigned int position_index = 0;

        while (position_index < 8) {
            if (SWITCH) {
                P2 = 0x00;
                P3 = Character[now_character_index];
                display_time++;
                if (display_time > 200) {
                    now_character_index = ++now_character_index % 16;
                    display_time = 0;
                }
            } else {
                display(position_index, Character[position_index]);
            }
            
            delay(1);
            position_index++;
        }
    }
}