#include <wiringPi.h>
#include <softTone.h>

#define SPKR 6
#define TOTAL 32

#define A 220.00
#define B 246.94
#define CC 261.63
#define DD 293.66
#define EE 329.63
#define FF 349.23
#define GG 391.00
#define GA 415.30
#define AA 440.00
#define BB 493.88
#define CCC 523.25
#define DDD 587.33
#define DDE 622.25
#define EEE 659.26
#define FFF 698.46
#define GGG 783.99
#define GGA 830.61
#define AAA 880.00
#define BBB 987.77
#define CCCC 1046.60

int notes[] = {
    BB, AA, GA, AA, CCC, 0, DDD, CCC, BB, CCC, 0, EEE, FFF, EEE, DDE, EEE,   
    BBB, AAA, GGA, AAA, BBB, AAA, GGA, AAA, CCCC, 0, AAA, 0, CCCC, 0, 
    GGG, AAA, BBB, 0, AAA, 0, GGG, 0, AAA, 0, 
    GGG, AAA, BBB, 0, AAA, 0, GGG, 0, AAA, 0,
    GGG, AAA, BBB, 0, AAA, 0, GGG, 0, FFF, 0 , EEE, EEE, 0,
    EEE, EEE, FFF, FFF, GGG, 0, GGG, 0, AAA, GGG, FFF, EEE, DDD, DDD, 
    EEE, EEE, FFF, FFF, GGG, 0, GGG, 0, AAA, GGG, FFF, EEE, DDD, DDD, 
    CCC, DDD, EEE, 0, EEE, 0, FFF, EEE, DDD, CCC, BBB, BBB,
    CCC, DDD, EEE, 0, EEE, 0, FFF, EEE, DDD, CCC, BBB, BBB,
    BB, AA, GA, AA, CCC, 0, DDD, CCC, BB, CCC, 0, EEE, FFF, EEE, DDE, EEE,   
    BBB, AAA, GGA, AAA, BBB, AAA, GGA, AAA, CCCC, 0, AAA, 0, CCCC, 0, 
    AAA, 0, BBB, 0, CCC, 0, BBB, 0, AAA, 0, GGA, 0, AAA, 0, EE, 0, FF, 0,
    CC, CC, B, CC, B, CC, B, A, B, A, 
    0
};

int musicPlay() {
    int i;
    softToneCreate(SPKR);

    for(i=0; i<TOTAL; ++i) {
        softToneWrite(SPKR, notes[i]);
        delay(100);
    }
    return 0;
}

int main(int argc, char **argv) {
    wiringPiSetup();
    musicPlay();
    return 0;
}
