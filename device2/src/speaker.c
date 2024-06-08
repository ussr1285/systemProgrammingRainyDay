#include <wiringPi.h>
#include <stdio.h>

// can modify these numbers
// GPIO 18
#define PIN 1

// function for speaker(buzzer)
int buzzer(void){
    // initialize wiringPi library to control GPIO pin
    wiringPiSetup();

    // set GPIO pin to pwm mode
    pinMode(PIN, PWM_OUTPUT);

    // set pwm clock: it affects the frequency of the pwm signal
    pwmSetClock(19);

    // set pwm mode to Mark-Space mode for more accurate pwm signal
    pwmSetMode(PWM_MODE_MS);

    // activate speaker 3 times
    for (int i = 0; i < 3; i++){
        // time pause for 1 sec
        delay(1000);

        // set pwm range to 262 hz (pitch A)
        pwmSetRange(1000000 / 262);

        // set pwm duty cycle
        pwmWrite(PIN, 1000000 / 262 / 2);

        // ringing for 3 sec
        delay(3000);

        // Stop ringing by setting the duty cycle to 0
        pwmWrite(PIN, 0);
    }

}

