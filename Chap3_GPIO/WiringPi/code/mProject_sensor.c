// INPUT: switch, light
//OUTPUT: LED, music 

#include <wiringPi.h>
#include <stdio.h>
#include <pthread.h>
#include <softTone.h>

#define SWITCH 5
#define LED 1
#define SPKR 6
#define CDS 0

int isAlarm = 1;
pthread_mutex_t  mid;

void *switch_func();
void *light_func();
void music_func(int);

int notes[] = {391, 440, 494, 440, 391};

int main(int argc, char **argv) {
    pthread_t ptSwitch, ptLight;
    pthread_mutex_init(&mid, NULL);

    pthread_create(&ptSwitch, NULL, switch_func, NULL);
    pthread_create(&ptLight, NULL, light_func, NULL);
    
    pthread_join(ptSwitch, NULL);
    pthread_join(ptLight, NULL);
            
    wiringPiSetup();

    pthread_mutex_destroy(&mid);

    return 0;
}

int changeAlarm(int prev) {
    if(prev == 0) return 1;
    else if(prev == 1) return 0;
    else {
        printf("Error : isAlarm value\n");
        return -1;
    }
}

void *switch_func() {
    pthread_mutex_lock(&mid);
    pinMode(SWITCH, INPUT);
    
    for(;;) {
        if(digitalRead(SWITCH) ==  LOW) {
            changeAlarm(isAlarm);
            if(isAlarm == 1) printf("Alarm ON\n");
            else if(isAlarm == 0) printf("Alarm OFF\n");
            else printf("Error : switch - isAlarm\n");
        }
    }
    pthread_mutex_unlock(&mid);
}

void *light_func() {
    pthread_mutex_lock(&mid);
    pinMode(CDS, INPUT);
    pinMode(LED, OUTPUT);

    for(;;) {
        if(digitalRead(CDS) == LOW) 
            digitalWrite(LED, HIGH);
        else {
            if(isAlarm == 1) music_func(isAlarm);
            if(isAlarm == 0) music_func(isAlarm);
        }
    }
    pthread_mutex_unlock(&mid);
}

void music_func(int current_alarm) {
    
    softToneCreate(SPKR);
    if(current_alarm == 1) {
        for(int i=0; i<5; ++i) {
            softToneWrite(SPKR, notes[i]);
            delay(200);
        }
    }
    else if(current_alarm == 0) {
        for(int i=0; i<5; ++i) {
            softToneWrite(SPKR,  0);
            delay(200);
        }
    }
    else {
        printf("Error : music_func\n");
        return;
    }
}
        
/* 빛 O
 *      알람O : 소리 켬
 *      알람X : 소리 끔
 * 빛 X
 *      불 켬
 * 스위치 -> 알람
 *
 */
