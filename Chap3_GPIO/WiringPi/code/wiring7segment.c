#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

int fndControl(int num) {
    int i;
    int gpiopins[7] = {8, 9, 7, 0, 15, 16, 1};
    int number[10][7] ={{0,0,0,0,0,0,1},
                        {1,0,0,1,1,1,1},
                        {0,0,1,0,0,1,0},
                        {0,0,0,0,1,1,0},
                        {1,0,0,1,1,0,0},
                        {0,1,0,0,1,0,0},
                        {0,1,0,0,0,0,0},
                        {0,0,0,1,1,1,1},
                        {0,0,0,0,0,0,0},
                        {0,0,0,0,1,0,0}};
    
    for(i=0; i<7; i++) {
        pinMode(gpiopins[i], OUTPUT);
    }

    for(i=0; i<7; i++) {
        digitalWrite(gpiopins[i], number[num][i]?HIGH:LOW); 
    }
    getchar();

    for(i=0; i<7; i++) {
        digitalWrite(gpiopins[i], HIGH);
    }
    
    return 0;
}

int main(int argc, char **argv) {
    int num;
    
    if(argc < 2) {
        printf("Usage: %s number\n", argv[0]);
        return -1;
    }
    
    num = atoi(argv[1]);
    wiringPiSetup();
    fndControl(num);

    return 0;
}

/* anode type
 * a    2  
 * b    3
 * c    4
 * d    17
 * e    14
 * f    15
 * g    18
 * vcc
 * */

