//NIME Final: Trigger Pad Project
const int NUM_PADS = 10;
const int TRANSISTOR_PIN[NUM_PADS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};    // pin transistor is connected to 
const int SENSOR_PIN[NUM_PADS] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9}; // pin FSR is connected to 
//
//const int analogPin = A1;
const int pwmPin = 3;

const int INVALID_THRESHOLD = 100;  // sensor values below this will be considered invalid
const int PRESSED_THRESHOLD = 620;  // the pad will be considered pressed when the sensor value is below this
const int RELEASED_THRESHOLD = 900;  // the pad will be considered released when the sensor value is above this

// Use typedefs to add synonyms for the names of types.  This can sometimes make your code easier to read.
// typedef unsigned long Milliseconds;
// Milliseconds currentTime = millis();

typedef unsigned long Milliseconds;

// We use enums for keeping track of state.  Add one enum value for each possible state.

typedef enum {
  STOPPED_AND_PAD_PRESSED,
  STOPPED_AND_PAD_RELEASED,
  PLAYING_AND_PAD_PRESSED,
  PLAYING_AND_PAD_RELEASED,
} State;
State currentState[NUM_PADS];

typedef enum {
  LIGHTS_ON,
  LIGHTS_OFF,
} CycleStage;
CycleStage currentStage[NUM_PADS];

Milliseconds currentCycleStartTime[NUM_PADS];
const Milliseconds END_OF_CYCLE = 500;

// In general, you should keep the time at which each stage ends in constants:
//
// const Milliseconds END_OF_STAGE_1 = 300;
// const Milliseconds END_OF_STAGE_2 = 700;
// const Milliseconds END_OF_STAGE_3 = 800;
const Milliseconds TIME_TO_SHUT_OFF_DA_LIGHTS = 150;

boolean padHasBeenReleased(int pad) {
   int sensorReading = analogRead(SENSOR_PIN[pad]);
//   Serial.print("pad released: ");
//   Serial.print(sensorReading, DEC);
//   Serial.println("");
   return sensorReading >= RELEASED_THRESHOLD;
}

boolean padHasBeenPressed(int pad) {
   int sensorReading = analogRead(SENSOR_PIN[pad]);
//   Serial.print("pad pressed: ");
//   Serial.print(sensorReading, DEC);
//   Serial.println("");
   return sensorReading <= PRESSED_THRESHOLD && sensorReading > INVALID_THRESHOLD;
}

void restartCycle(int pad) {
//  Serial.print(TRANSISTOR_PIN[pad], DEC);
//  Serial.println(" light on");
  digitalWrite(TRANSISTOR_PIN[pad], HIGH);
  currentStage[pad] = LIGHTS_ON;
  currentCycleStartTime[pad] = millis();
}

boolean playingLoop(int pad) {
  const Milliseconds timeInCycle = millis() - currentCycleStartTime[pad];

  switch (currentStage[pad]) {
    
    case LIGHTS_ON:
      if (timeInCycle > TIME_TO_SHUT_OFF_DA_LIGHTS) {
//        Serial.print(TRANSISTOR_PIN[pad], DEC);
//        Serial.println(" light off");
        digitalWrite(TRANSISTOR_PIN[pad], LOW);
        currentStage[pad] = LIGHTS_OFF;
      }
      break;
      
    case LIGHTS_OFF:
      if (timeInCycle > END_OF_CYCLE) {
        restartCycle(pad);
      }
      break;     
  }
}



void setup() {
  Serial.begin(9600);       // use the serial port
  
  pinMode(pwmPin,OUTPUT);
  
  // Wrap everything below in for-loop in order to support multiple pads:
  for (int i = 0; i < NUM_PADS; i++) {
    pinMode(TRANSISTOR_PIN[i], OUTPUT);
    digitalWrite(TRANSISTOR_PIN[i], LOW);
    currentStage[i] = LIGHTS_OFF;
    currentState[i] = STOPPED_AND_PAD_RELEASED;
  }
}


void loop() {
  
  //delay(10); // prevents the Serial port from being over killed
  
  // read the analog pad, and prepend it with a 9
//  Serial.print(9);
//  Serial.print(' ');
//  Serial.print(analogRead(analogPin));
//  Serial.println();
//  
//  int sensorValue = analogRead(analogPin);
//  int pwmValue = map(sensorValue,100, 600, 0, 255);
//  analogWrite(pwmPin,pwmValue);
  
  
  // Wrap everything in for-loop in order to support multiple pads:
  for (int i = 0; i < NUM_PADS; i++) {
    switch (currentState[i]) {
  
      case STOPPED_AND_PAD_PRESSED:
        if (padHasBeenReleased(i)) {
          currentState[i] = STOPPED_AND_PAD_RELEASED;
        }
        break;
        
      case STOPPED_AND_PAD_RELEASED:
        if (padHasBeenPressed(i)) {
          Serial.print(i, DEC);
          Serial.print(" ");
          Serial.print(1, DEC);
          Serial.println(" ");
          restartCycle(i);
          currentState[i] = PLAYING_AND_PAD_PRESSED;
        }
        break;
        
      case PLAYING_AND_PAD_PRESSED:
        playingLoop(i);
        if (padHasBeenReleased(i)) {
          currentState[i] = PLAYING_AND_PAD_RELEASED;
        }
        break;
        
      case PLAYING_AND_PAD_RELEASED:
        playingLoop(i);
        if (padHasBeenPressed(i)) {
          Serial.print(i, DEC);
          Serial.print(" ");
          Serial.print(0, DEC);
          Serial.println(" ");
          digitalWrite(TRANSISTOR_PIN[i], LOW);
          currentState[i] = STOPPED_AND_PAD_PRESSED;
        }
        break;
        
    }
  }  
}



