#include <wiringPi.h>
#include <stdio.h>

// GPIO 1
#define PIN 1 // can modify this number

int buzzer(void){
    wiringPiSetup();

    pinMode(PIN, PWM_OUTPUT);

    pwmSetClock(19);
    pwmSetMode(PWM_MODE_MS);

    pwmSetRange(1000000 / 262);
    pwmWrite(PIN, 1000000 / 262 / 2);
    delay(3000);

    pwmWrite(PIN, 0);
}

