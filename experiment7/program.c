#include <reg51.h>
#include <intrins.h>

sbit SWITCH = P0^0;
sbit SOUNDER = P0^7;

sbit C1 = P2^4;
sbit C2 = P2^5;
sbit C3 = P2^6;
sbit C4 = P2^7;

unsigned char code NoteDisplay[] = {
    0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07
};

unsigned int code TimerReload[14] = {
    0xF88C, 0xF95B, 0xFA15, 0xFA68, 0xFB04, 0xFB90, 0xFC0C,
    0xFC44, 0xFCAC, 0xFD09, 0xFD34, 0xFD82, 0xFDC8, 0xFE06
};

unsigned int code Notes[7] = {
    0xFC44, 0xFCAC, 0xFD09, 0xFD34, 0xFD82, 0xFDC8, 0xFE06
};

unsigned char code MusicScore[] = {
    0x01, 0x01, 0x51, 0x51, 0x61, 0x61, 0x51, 0x04,
    0x41, 0x41, 0x31, 0x31, 0x21, 0x21, 0x11, 0x04,
    0x51, 0x51, 0x41, 0x41, 0x31, 0x31, 0x21, 0x04,
    0x51, 0x51, 0x41, 0x41, 0x31, 0x31, 0x21, 0x04,
    0x01, 0x01, 0x51, 0x51, 0x61, 0x61, 0x51, 0x04,
    0x41, 0x41, 0x31, 0x31, 0x21, 0x21, 0x11, 0x0F
};

unsigned int input_delay = 1000;
unsigned int key = 16;

bit music_mode = 0;
unsigned char music_index = 0;
unsigned int music_tick = 0;
unsigned int music_duration = 0;
unsigned char current_note = 16;

typedef enum {
    MUSIC_IDLE,
    MUSIC_PLAY_NOTE,
    MUSIC_NOTE_GAP,
    MUSIC_END
} MusicState;

MusicState music_state = MUSIC_IDLE;

void on_timer_timeout(void) interrupt 1 {
    if(music_mode) {
        if(current_note < 16) {
            SOUNDER = !SOUNDER;
            TH0 = (Notes[current_note] >> 8) & 0xFF;
            TL0 = Notes[current_note] & 0xFF;
        } else {
            SOUNDER = 1;
            TH0 = 0xFC;
            TL0 = 0x18;
        }
    } else {
        if(key < 16) {
            SOUNDER = !SOUNDER;
            TH0 = (TimerReload[key] >> 8) & 0xFF;
            TL0 = TimerReload[key] & 0xFF;
        } else {
            SOUNDER = 1;
            TH0 = 0xFC;
            TL0 = 0x18;
        }
    }
}

void exit_music_mode(void) {
    music_mode = 0;
    music_state = MUSIC_IDLE;
    music_index = 0;
    current_note = 16;
    SOUNDER = 1;
    P3 = 0x00;
    TH0 = 0xFC;
    TL0 = 0x18;
}

void play_music_note(unsigned char music_data) {
    current_note = (music_data >> 4) & 0x0F;
    
    switch(music_data & 0x0F) {
        case 1: music_duration = 200; break;
        case 2: music_duration = 100; break;
        case 4: music_duration = 400; break;
        default: music_duration = 200; break;
    }
    
    if(current_note < 7) {
        P3 = NoteDisplay[current_note];
    } else {
        P3 = 0x00;
    }
}

void music_player(void) {
    unsigned int i;
    unsigned char music_data;
    
    if(!SWITCH) {
        exit_music_mode();
        return;
    }
    
    if(music_index >= sizeof(MusicScore)) {
        exit_music_mode();
        return;
    }
    
    switch(music_state) {
        case MUSIC_IDLE:
            music_data = MusicScore[music_index];
            
            if(music_data == 0x0F) {
                music_index = 0;
                return;
            }
            
            play_music_note(music_data);
            music_tick = 0;
            music_state = MUSIC_PLAY_NOTE;
            break;
            
        case MUSIC_PLAY_NOTE:
            if(music_tick < music_duration) {
                for(i = 0; i < 100; i++);
                music_tick++;
            } else {
                SOUNDER = 1;
                P3 = 0x00;
                music_tick = 0;
                music_state = MUSIC_NOTE_GAP;
            }
            break;
            
        case MUSIC_NOTE_GAP:
            for(i = 0; i < 50; i++);
            music_index++;
            music_state = MUSIC_IDLE;
            break;
            
        default:
            break;
    }
}

void main() {
    unsigned int input_ticks = 0;
    unsigned int row = 0;
    unsigned int i;
    
    TMOD = 0x01;
    TH0 = 0xFC;
    TL0 = 0x18;
    ET0 = 1;
    EA = 1;
    TR0 = 1;
    
    P1 = 0x00;
    
    while(1) {
        if (SWITCH) {
            SOUNDER = 1;
            music_mode = 1;
            key = 16;
            music_state = MUSIC_IDLE;
            music_index = 0;
            current_note = 16;
            
            while(SWITCH && music_mode) {
                music_player();
            }
            
            if(music_mode) {
                exit_music_mode();
            }
        }
        
        if (!music_mode) {
            if (input_ticks++ > input_delay) {
                input_ticks = 0;
                row = row % 4;
                P2 = _crol_(~0x01, row);
                
                if(C1 == 0) key = row * 4 + 0;
                if(C2 == 0) key = row * 4 + 1;
                if(C3 == 0) key = row * 4 + 2;
                if(C4 == 0) key = row * 4 + 3;
                row++;
            }
            
            if (key < 16) {
                P3 = NoteDisplay[key % 7];
            } else {
                P3 = 0x00;
            }
        } else {
            if (input_ticks++ > input_delay) {
                input_ticks = 0;
                row = row % 4;
                P2 = _crol_(~0x01, row);
                row++;
            }
        }
    }
}