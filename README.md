# system programming and practice: Team project
Team name: RainyDay


# Device 2
Author: Junsung Kim


# Description of device 2
Device2 controls 1 sensor and 1 actuator
Sensor: temperature and humidity sensor (dht11)
Actuator: speaker (piezo buzzer)


# For using sensor
The order of pin: vcc(+) - i/o - ground(-)
The written code uses GPIO 4


# For using actuator
The order of pin: vcc(+) - i/o - ground(-)
The written code uses GPIO 18


# Execute this code
Notice: need sudo permission for executing code
~/device2 $ cd exe
~/device2/exe $ gcc -c ../src/speaker.c
~/device2/exe $ gcc -c ../src/temperature_and_humidity.c
~/device2/exe $ gcc -c ../src/
~/device2 $ gcc -o client -l wiringPi
~/device2 $ ./client IP PORT
