## Kitchen timer based on ultrasonic distance detection
This is a kitchen timer project. It works via ultrasonic distance detection. That means you do not have to touch any buttons to activate a timer. The ultrasonic sensor is controlled via an Arduino board.
The sensor sends out ultrasonic sound waves. After a specified amount of time we stop and start receiving the returning waves. Using the time dilation we can calculate the approximate distance of the sensor to other objects. To avoid fluctuations in the measurement we use an average over multiple measurements
The board also controls a set of status LEDs. They are used to indicate the setup process and the length of the timer.

**Features**
- detect objects (e.g. your hand)
- calculate the distance between the object and itself
- only start timer process if object is near the device
- set two different timers
- play a sound if timer is up


**How to use it:**
Place your hand in front of the ultrasonic sensor. If it detects an object within a ~60cm range it will start the timer setting process. If you continue to hold, first the green light will light up (3 minute timer). After two additional seconds the yellow light will light up (6 minute timer). Once you remove your hand the timer automatically starts. You will hear a beeping noise from the active buzzer once the timer is up. Afterwards the timer automatically resets itself. You can remove you hand any time during the setup process to restart the process.

***Hardware setup:***
Here is list of the hardware that I used for this project:
1. Arduino Board
2. 1x Active Buzzer
3. 4 LEDs (white, green, yellow, red)
4. Ultrasonic Sensor
5. A few resistors and cables

There are many ways to connect the ultrasonic sensor and LEDs to an Arduino board. You can find many tutorials on the internet. Just adjust the pins in the setup accordingly.

***Software:***
The provided code works out of the box. It has a few parameters that can be customized e.g. the initial level of difficulty.
Here is a UML State Diagram outlining the behavior of the machine:
![UltrasonicTimer](https://user-images.githubusercontent.com/42205689/148792329-b131071a-0048-4cc2-9d61-9d86d2fa24b5.png)

**Limitations:**

For my specific use case I only need two different timer values i.e. 3 minutes and 6 minutes. That is why my setup only supports setting two different timers. However if you look at the state diagram it is easy to see how one could expand on that if necessary.
