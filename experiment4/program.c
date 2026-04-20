#include<reg51.h>
#include<intrins.h>

sbit SWITCH = P0^0;

sbit C1 = P2^4;
sbit C2 = P2^5;
sbit C3 = P2^6;
sbit C4 = P2^7;

unsigned char code character[] = {
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
    0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71
};

unsigned char code position[] = {0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};
unsigned char buffer[8] = {16};
unsigned char index = 0;
unsigned char has_pressed = 0;
unsigned char last_mode = 0;

void delay(unsigned int time) {
    unsigned long ticks = 50 * time;
    while(ticks-- > 0);
}

unsigned char scan() {
    unsigned int i = 0;
    unsigned char key = 16;
    unsigned char row[] = {0xFE,0xFD,0xFB,0xF7};
    
    while(i < 4) {
        P2 = row[i];
        delay(1);
        if(C1 == 0) key = i*4 + 0;
        if(C2 == 0) key = i*4 + 1;
        if(C3 == 0) key = i*4 + 2;
        if(C4 == 0) key = i*4 + 3;
        if(key != 16) break;
        i++;
    }
    P2 = 0xFF;
    return key;
}

void add_key_to_bufferfer(unsigned char key) {
    unsigned int i = 0;
    while(i < 7) { 
        buffer[i] = buffer[i+1];
        i++;;
    }
    buffer[7] = key;
    
    has_pressed = 0;
    while(i < 8) {
        if(buffer[i] != 16) {
            has_pressed = 1;
            break;
        }
        i++;
    }
}

void main() {
    unsigned char key, last = 16;
    unsigned char i;
    
    for(i = 0; i < 8; i++) buffer[i] = 16;
    last_mode = SWITCH;
    
    while(1) {
        key = scan();
        
        if(key != 16 && last == 16) {
            delay(1);
            key = scan();
            if(key != 16) {
                add_key_to_bufferfer(key);
            }
        }
        
        last = key;
        
        if(has_pressed) {
            for(i = 0; i < 8; i++) {
                P1 = position[i];
                if(SWITCH == 1) {
                    if(buffer[7] != 16)
                        P3 = character[buffer[7]];
                    else
                        P3 = 0x00;
                } else {
                    if(buffer[i] != 16)
                        P3 = character[buffer[i]];
                    else
                        P3 = 0x00;
                }
                delay(1);
            }
        } else {
            P1 = 0xFF;
            P3 = 0x00;
        }
    }
}