What is it
In order to convert a regular bicycle to an e-bike, one needs a microcontroller to regulate load flow and motor speed. 
It should take in a signal from a throttle (in this case we used a joystick) and increase speed as the joystick displacement increases.

How to use it
Onboard microcontroller - allows for remote control of a motor via MQTT.
Additionally, using MQTT protocol, incoming data from this onboard microcontroller gets published to a webpage. 
This data includes the temperature of motor (thermistor), rotations per minute, and proximity of physical objects to front of bike(ultrasonic sensor).
In the event that an object gets too close, a buzzer alarms the rider (collision detection).

https://github.com/sham4sh/csc113-controller

