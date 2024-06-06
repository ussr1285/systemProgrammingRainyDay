#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// GPIO 7
#define T_H_PIN 7 // can modify this number

int data[5];

int readData(void){
    uint8_t laststate = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0;

    for (int i = 0; i < 5; i++)
        data[i] = 0;

    pinMode(T_H_PIN, OUTPUT);
    digitalWrite(T_H_PIN, LOW);
    delay(18);
    digitalWrite(T_H_PIN, HIGH);
    delayMicroseconds(40);
    pinMode(T_H_PIN, INPUT);

    for (uint8_t i = 0; i < 100; i++){
        counter = 0;

        while (digitalRead(T_H_PIN) == laststate){
            counter++;
            delayMicroseconds(1);
            if (counter == 255)
                break;
        }

        laststate = digitalRead(T_H_PIN);

        if (counter == 255)
            break;

        if ((i >= 4) && (i % 2 == 0)){
            data[j/8] <<= 1;
            if (counter > 16)
                data[j/8] |= 1;
            j++;
        }
    }

    if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))){
        return 1;
    }

    return 0;
}

int t_and_h(void){
    while (wiringPiSetup() == -1);
    while (readData() == 0)
        delay(1000);

    //printf("humidity: %d.%d %% temperature: %d.%d C\n", data[0], data[1], data[2], data[3]);

    return data[0];
}
