/**
 * 
 */

#define BUZZER_PIN  3
#define WHITE_PIN 10
#define GREEN_PIN 11
#define YELLOW_PIN 12
#define RED_PIN 13
#define SOUND_IN_MS 0.0297
#define MEAN_LENGTH 10
#define SLEEP_STATE 0
#define READY_STATE 1
#define TIMER_SETTING_START_STATE 2
#define GREEN_SET_ORANGE_WATING_STATE 3
#define ORANGE_SET_STATE 4
#define TIMER_RUNNING_STATE 5
#define TRIGGER_PIN 9
#define ECHO_PIN 8
#define DISTANCE_MEASUREMENT_DELAY 10

//methods
void configureUltraSonicSensor();
int calculateDistance();
void configureLEDs();
void blinkLEDs();
void updateStates(int i);
void controlTimer();
void reset();

// Ultrasonic sensor setup
float duration_ms, distance_cm;

// Ultrasonic distance indicator leds
int distanceLeds[] = {10,11,12};
int ultrasonicStatusLED = 2;
int white_ledState = HIGH;
int green_ledState = LOW;
int yellow_ledState = LOW;
int red_ledState = LOW;

bool white_blinking = false;
bool green_blinking = false;
bool yellow_blinking = false;
bool red_blinking = false;

// init array with zeros by aggregation-initialization
int meanMeasurementArray[MEAN_LENGTH] = {};
int mean_index = 0;
int loop_index = 0;
int distance_in_cm_average = 0;
int state = SLEEP_STATE;
long timer = 0;
bool bigTimer = false;
bool handDetected = false;
int timeSinceHandInMs = 0;

void setup() {
  // setup serial port
  Serial.begin (9600);
  // setup for all sensors and motors
  configureUltraSonicSensor();
  configureLEDs();

  //setup buzzer
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // get current distance via ultrasonic measurement
  distance_cm = calculateDistance();

  // check every second
  if (loop_index >= 100) {
    loop_index = 0;
    if (handDetected   && distance_cm < 60) {
      timeSinceHandInMs +=10;
    } else if (distance_cm < 60) {
        handDetected = true;
        timeSinceHandInMs = 0;
    } else {
       handDetected = false;
       timeSinceHandInMs = 0;
    }
    Serial.print("Mean distance: ");
    Serial.println(distance_cm);
  }
  
  // do every 500ms
  if (loop_index == 50 || loop_index == 0) {
      blinkLEDs();
      // update LEDs according to their state
      digitalWrite(WHITE_PIN, white_ledState);
      digitalWrite(GREEN_PIN, green_ledState);
      digitalWrite(YELLOW_PIN, yellow_ledState);
      digitalWrite(RED_PIN, red_ledState);
      updateStates(timeSinceHandInMs);
  }
  
  if (state == TIMER_RUNNING_STATE) {
    controlTimer();
  }
  loop_index++;
  delay(10);
}


/**
 * Update all steps based on the duration of the hand detection 
 */
void updateStates(int timeSinceHandInMS){
    if (state == SLEEP_STATE && handDetected) {
        state = READY_STATE;
        white_blinking = true;
    } else if (state == READY_STATE && timeSinceHandInMS > 10 && handDetected) {
        state = TIMER_SETTING_START_STATE;
        green_blinking = true; 
    } else if (state == TIMER_SETTING_START_STATE && timeSinceHandInMS > 30 && handDetected) {
        state = GREEN_SET_ORANGE_WATING_STATE;
        green_blinking = false;
        yellow_blinking = true;
    } else if (state == GREEN_SET_ORANGE_WATING_STATE && timeSinceHandInMS > 50 && handDetected  ) {
        state = ORANGE_SET_STATE;
        yellow_blinking = false;
        // todo: add yellow lighting all the time state here
    } else if (state == GREEN_SET_ORANGE_WATING_STATE && !handDetected) {
        state = TIMER_RUNNING_STATE;
        white_blinking = false;
        red_blinking = true;
        timer = millis();
        bigTimer = false;
    } else if (state == ORANGE_SET_STATE && !handDetected) {
        state = TIMER_RUNNING_STATE;
        white_blinking = false;
        red_blinking = true;
        timer = millis();
        bigTimer = true;
    } else if (!handDetected && state != TIMER_RUNNING_STATE) {
       // no hand detected -> sleepState gets activated
       reset();
    }
}

/**
 * Setup configuration
 */
void configureUltraSonicSensor(){
  // configure trigger pin to output mode
  pinMode(TRIGGER_PIN, OUTPUT);
  // configure echo pin to input mode
  pinMode(ECHO_PIN, INPUT);

}

/**
 * Control the timer and trigger the buzzer once the timer is up
 */
void controlTimer(){
  if ((bigTimer && millis() > timer + 360000) || (millis() > timer + 180000 && !bigTimer)) {
      Serial.println("timer is over");
      reset();

      // trigger buzzer
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      delay(500);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      delay(500);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
  } 
}

/**
 * Reset the timer and all LEDs to the sleep state configuration
 */
void reset(){
    state = SLEEP_STATE;
    white_blinking = false;
    green_blinking = false;
    yellow_blinking = false;
    red_blinking = false;
    bigTimer = false;
    timer = 0;
}

/**
 * Calculate the average of the last #MEAN_LENGTH measurements
 */
int calculateDistance() {
  // 10ms pulse to trig pin
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(DISTANCE_MEASUREMENT_DELAY);
  digitalWrite(TRIGGER_PIN, LOW);
  
  // measure duration of pulse from ECHO pin
  duration_ms = pulseIn(ECHO_PIN, HIGH);
  // distance calculation
  distance_cm = (duration_ms/2) * SOUND_IN_MS;

  meanMeasurementArray[mean_index] = distance_cm;
  if (mean_index++ == MEAN_LENGTH) {
     // use average to remove fluctuations in measurements
     mean_index = 0;
     int sum = 0;
     distance_in_cm_average = 0;
     for (int measurement : meanMeasurementArray) {
         sum += measurement;
     }
     distance_in_cm_average = floor(sum/MEAN_LENGTH);
   }
   return distance_in_cm_average;
}

/**
 * Setup configuration
 */
void configureLEDs(){
  // configure led pin modes
  for(int & distanceLed : distanceLeds){
      pinMode(distanceLed, OUTPUT);
  }
  pinMode(ultrasonicStatusLED, OUTPUT);
  digitalWrite(ultrasonicStatusLED, HIGH);
}

/**
 * Regulate the blinking of leds
 */
void blinkLEDs(){
    //white
    if (white_blinking   && white_ledState == HIGH || state == SLEEP_STATE) {
          white_ledState = LOW;
    } else if(white_blinking   || state == TIMER_RUNNING_STATE) {
        white_ledState = HIGH;
    }
  
    //green
    if ((green_blinking   && green_ledState == HIGH) || state == SLEEP_STATE) {
          green_ledState = LOW;
    } else if(green_blinking   || state == GREEN_SET_ORANGE_WATING_STATE) {
        green_ledState = HIGH;
    }
  
    //yellow
    if ((yellow_blinking && yellow_ledState == HIGH) || state == SLEEP_STATE
            || (state == TIMER_RUNNING_STATE && !bigTimer)) {
          yellow_ledState = LOW;
    } else if(yellow_blinking   || state == ORANGE_SET_STATE) {
        yellow_ledState = HIGH;
    }
  
    //red
    if (red_blinking && red_ledState == HIGH || state == SLEEP_STATE) {
          red_ledState = LOW;
    } else if(red_blinking  ) {
        red_ledState = HIGH;
    }
}
