#include "MAX6675.h"
#include "ReflowGUI.h" 

/*

Reflow Hot Plate Firmware
Written by Christopher Yue

*/

// PIN DESIGNATION
#define SW_UP 2
#define SW_DOWN 3
#define SW_NEXT 4
#define RELAY_N 5
#define RELAY_P 6
#define SERIAL_INPUT 7 // SO pin of MAX6675
#define SERIAL_OUT 8 // SER pin of 74HC595
#define SRCLK 9 // shift register clock 74HC595
#define RCLK 10 // register clock 74HC595
#define CHIP_SELECT 12 // CS pin MAX6675
#define SERIAL_CLOCK 13 // SCK pin MAX6675

// #define MOTOR_EN 5
// #define MOTOR_A A2
// #define MOTOR_B A1

// PARAMETERS
// default HEATING_PARAMETERS
// tuned for ChipQuik NC191SNL15 solder paste
#define PREHEAT_TEMP 150.0
#define PREHEAT_DURATION 90.0
#define SOAK_TEMP 175.0
#define SOAK_DURATION 90.0
#define MELTING_POINT 217.0
#define RAMP_DURATION 30.0
#define REFLOW_TEMP 249.0
#define REFLOW_DURATION 60.0

#define PWM_PERIOD 2000 // ms

// PID tuning - KU is the value at constant oscillations, TU is the period
#define KU 0.05
#define TU 12

// PID - zieger-nichols method
//#define KP 0.6*KU
#define TI 0.5*TU
//#define TD 0.125*TU
#define KP 0.05
#define TD 7
// not needed - can simply use standard form of PID equation
// #define KI KP/TI
// #define KD KP*TD

#define PREHEAT 0
#define SOAK 2
#define MELT 4
#define REFLOW 6

#define HEATING_BUFFER_THRESHOLD 1.5
#define CURVE_CONSTANT 10.0

float HEATING_PARAMETERS[8] = {PREHEAT_TEMP, PREHEAT_DURATION, SOAK_TEMP, SOAK_DURATION, MELTING_POINT, RAMP_DURATION, REFLOW_TEMP, REFLOW_DURATION};
const float INCREMENTS[8] = {2.5, 5, 2.5, 5, 2.5, 5, 2.5, 5};
float HEATING_RATES[4] = {PREHEAT_TEMP/PREHEAT_DURATION, (SOAK_TEMP-PREHEAT_TEMP)/SOAK_DURATION, (MELTING_POINT-SOAK_TEMP)/RAMP_DURATION, (REFLOW_TEMP-MELTING_POINT)/(REFLOW_DURATION/2)};

struct Point {
    float x;
    float y;
};

// Bezier smoothing algorithm, sourced mainly using AI
class BezierCorner {
  private:
    Point p0, p1, p2, p3;

    float formula(float coord0, float coord1, float coord2, float coord3, float t) {
        float u = 1.0 - t;
        return (u*u*u * coord0) + (3.0 * u*u * t * coord1) + (3.0 * u * t*t * coord2) + (t*t*t * coord3);
    }

  public:
    void setup(Point start, Point ctrl1, Point ctrl2, Point end) {
        p0 = start; p1 = ctrl1; p2 = ctrl2; p3 = end;
    }

    float getTemperatureAtTime(float t) { // as a decimal between 0.0 and 1.0
      return formula(p0.y, p1.y, p2.y, p3.y, t);
    }
};

BezierCorner smoothCorners[5];

MAX6675 thermocouple(SERIAL_CLOCK, CHIP_SELECT, SERIAL_INPUT);
LCD1602 display(SRCLK, RCLK, SERIAL_OUT); // controlled using 74HC595, RS is B, R/W is C, E is D, data pins 4-7 correspond to E-H
ReflowGUI gui(display);

bool HEATING = false;
bool STATE_SW_NEXT = false;

bool hasAdvanced = false;
bool check_gui = true;
volatile bool hasEdited = false;
bool heating_buffer_active = false; 

long time;
long START;
long prevButtonCheckTime = 0;
long prevCycleTime = 0;
float heating_start_time = -1;
float heating_duration = -1.0;
float nominal_heating_rate = 0.0;

int phase = PREHEAT; // preheat, soak, reflow, cool
float targetTemp;
float temp;
float prevError = 0;

float DUTY_CYCLE;
float ie;

// bool hasIncremented = false;
// bool hasIncremented = false;

float updateHeatingRates() {
  HEATING_RATES[0] = (HEATING_PARAMETERS[PREHEAT]-temp)/HEATING_PARAMETERS[1];
  for (int i = SOAK; i < REFLOW; i+=2) {
    HEATING_RATES[(int)i/2] = (float)(HEATING_PARAMETERS[i] - HEATING_PARAMETERS[i-2]) / HEATING_PARAMETERS[i+1];
  }
  HEATING_RATES[(int)REFLOW/2] = (float)(HEATING_PARAMETERS[REFLOW] - HEATING_PARAMETERS[MELT]) / (HEATING_PARAMETERS[REFLOW+1]/2);
}

void incrementValue() {
  if (!digitalRead(SW_UP)) {
    if (!hasEdited) {
      HEATING_PARAMETERS[gui.screen]+=INCREMENTS[gui.screen];
      gui.edit_param(HEATING_PARAMETERS[gui.screen]);
      hasEdited = true;
    }
  } else {
    hasEdited = false;
  }
  delay(50);
}

void decrementValue() {
  if (!digitalRead(SW_DOWN)) {
    if (!hasEdited) {
      HEATING_PARAMETERS[gui.screen]-=INCREMENTS[gui.screen];
      gui.edit_param(HEATING_PARAMETERS[gui.screen]);
      hasEdited = true;
    }
  } else {
    hasEdited = false;
  }
  delay(50);
}

void nextValue() {
  if (gui.advance_screen(gui.screen >= 7 ? NULL : HEATING_PARAMETERS[gui.screen+1])) {
    temp = thermocouple.readData();
    start();
    return;
  }
  delay(50);
}

void setupCornerSmoothing() {

  // 0: start to preheat
  // 1: preheat to soak
  // 2: soak to melt
  // 3: melt to reflow
  // 4: reflow to cool


  smoothCorners[0].setup(
    {0, temp+HEATING_BUFFER_THRESHOLD},
    {CURVE_CONSTANT/2, temp+HEATING_BUFFER_THRESHOLD},
    {CURVE_CONSTANT/2, temp+HEATING_BUFFER_THRESHOLD},
    {CURVE_CONSTANT, temp+CURVE_CONSTANT*HEATING_RATES[0]}
  );

  smoothCorners[4].setup(
    {-CURVE_CONSTANT, HEATING_PARAMETERS[REFLOW] - CURVE_CONSTANT*HEATING_RATES[3]},
    {-CURVE_CONSTANT/2, HEATING_PARAMETERS[REFLOW] - (CURVE_CONSTANT/2)*HEATING_RATES[3]},
    {-CURVE_CONSTANT/2, HEATING_PARAMETERS[REFLOW]},
    {0, HEATING_PARAMETERS[REFLOW]}
  );

  for (int i = 1; i < 4; i++) {
    // soak -> melt -> reflow
    smoothCorners[i].setup(
      {-CURVE_CONSTANT, HEATING_PARAMETERS[2*(i-1)]-CURVE_CONSTANT*HEATING_RATES[i-1]},
      {-CURVE_CONSTANT/2, HEATING_PARAMETERS[2*(i-1)]-(CURVE_CONSTANT/2)*HEATING_RATES[i-1]},
      {CURVE_CONSTANT/2, HEATING_PARAMETERS[2*(i-1)]+(CURVE_CONSTANT/2)*HEATING_RATES[i]},
      {CURVE_CONSTANT, HEATING_PARAMETERS[2*(i-1)]+CURVE_CONSTANT*HEATING_RATES[i]}
    );
  }

  for (int i = 0; i < 5; i++) {
    Serial.println(i);
    for (float j = 0.0; j <= 1.0; j+=0.1) {
      Serial.print(smoothCorners[i].getTemperatureAtTime(j));
      Serial.print(", ");
    }
  }

}

float getTargetTempAt(long elapsedTimeInPhase, int phase, long duration) {
  if (phase == PREHEAT && elapsedTimeInPhase <= CURVE_CONSTANT*2000) {
    return smoothCorners[(int)phase/2].getTemperatureAtTime(elapsedTimeInPhase/(2*CURVE_CONSTANT*1000));
  }
  if (elapsedTimeInPhase <= CURVE_CONSTANT*1000) {
    float t = min(1.0, elapsedTimeInPhase/(2*CURVE_CONSTANT*1000)+0.5);
    t = round(t*CURVE_CONSTANT)/CURVE_CONSTANT;
    Serial.print("Bezier smoothing pre ");
    Serial.println(t);
    return smoothCorners[(int)phase/2].getTemperatureAtTime(t);
  } else if (elapsedTimeInPhase >= (duration-CURVE_CONSTANT)*1000) {
    float t = (elapsedTimeInPhase/1000.0+CURVE_CONSTANT-duration)/(2*CURVE_CONSTANT);
    t = round(t*CURVE_CONSTANT)/CURVE_CONSTANT;
    Serial.print("Bezier smoothing post ");
    Serial.println(t);
    return smoothCorners[((int)phase/2)+1].getTemperatureAtTime(t);
  }
  return -1;
}

void start() {
  detachInterrupt(digitalPinToInterrupt(SW_UP));
  detachInterrupt(digitalPinToInterrupt(SW_DOWN));

  HEATING = true;
  check_gui = false;
  DUTY_CYCLE = 0.5;
  heating_start_time = time;
  heating_duration = HEATING_PARAMETERS[1];
  phase = PREHEAT;

  temp = thermocouple.readData();
  targetTemp = temp + HEATING_RATES[0];

  updateHeatingRates();
  setupCornerSmoothing();

  nominal_heating_rate = (smoothCorners[1].getTemperatureAtTime(0.0) - smoothCorners[0].getTemperatureAtTime(1.0))/(HEATING_PARAMETERS[1]-3*CURVE_CONSTANT);

  gui.printHeating(phase);
}

float cmap(float val, float olow, float ohigh, float mlow, float mhigh) {
  return constrain(map(val, olow, ohigh, mlow, mhigh), mlow, mhigh);
}

// void motorSpeed(float val) {
//   int map_speed = cmap(abs(val), 0, 100, 0, 255);

//   if (val > 0) {
//     digitalWrite(MOTOR_A, HIGH);
//     digitalWrite(MOTOR_B, LOW);
//   }
//   else {
//     digitalWrite(MOTOR_A, LOW);
//     digitalWrite(MOTOR_B, HIGH);
//   }

//   analogWrite(MOTOR_EN, map_speed);
// }

void reset() {
  phase=PREHEAT;
  HEATING = false;
  gui.screen = 0;
  gui.update(HEATING_PARAMETERS[0]);
  check_gui = true;
  attachInterrupt(digitalPinToInterrupt(SW_UP), incrementValue, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW_DOWN), decrementValue, CHANGE);
  digitalWrite(RELAY_P, LOW);
  heating_buffer_active = true;
  //motorSpeed(0);
}

void updateHeating() {

  if (time - prevCycleTime >= DUTY_CYCLE*PWM_PERIOD) {
    digitalWrite(RELAY_P, LOW);
  }

  if (time - prevCycleTime >= PWM_PERIOD) {

    // end of a period

    // PID implementation

    // first check how much we are off by - positive means too low, negative means too high
    float error = targetTemp-temp; // C
    float de = (error-prevError); // C/s, if error is increasing this is positive meaning that we need to heat faster, if error is decreasing this is negative meaning we need to heat slower
    ie += (error+prevError/2); // rough estimation, we can consider each new point as adding a triangle and a rectangle

    DUTY_CYCLE += KP*(error+TD*de);
    if (DUTY_CYCLE < 0) DUTY_CYCLE = 0;
    else if (DUTY_CYCLE > 1) DUTY_CYCLE = 1;

    // print data
    Serial.print("Time:");
    Serial.print(time);
    Serial.print(",");
    Serial.print("Target:");
    Serial.print(targetTemp);
    Serial.print(",");
    Serial.print("Actual:"); 
    Serial.print(temp);
    Serial.print(",");
    Serial.print("Error:");
    Serial.println(error);
    
    if (time-heating_start_time >= heating_duration*1000) {
      phase+=2;
      heating_start_time = time;
      heating_duration = phase == REFLOW ? HEATING_PARAMETERS[phase+1]/2 : HEATING_PARAMETERS[phase+1];

      float t_next = smoothCorners[(int)phase/2+1].getTemperatureAtTime(0.0);
      float t_start = smoothCorners[(int)phase/2].getTemperatureAtTime(1.0);

      nominal_heating_rate = heating_duration-CURVE_CONSTANT*2 <= 0.25 ? 0.0 : (t_next-t_start)/(heating_duration-CURVE_CONSTANT*2);
      Serial.print("Nominal rate ");
      Serial.println(nominal_heating_rate);
      if (phase > REFLOW) {
        reset();
        return;
      }
    }

    float targetTemp_temp = targetTemp;
    targetTemp = getTargetTempAt(time-heating_start_time, phase, heating_duration);
    
    if (targetTemp + 1 <= 0.05) { // floating point issues
      targetTemp = min(targetTemp_temp + nominal_heating_rate * (PWM_PERIOD/1000), HEATING_PARAMETERS[phase]);
    }

    if (DUTY_CYCLE > 0) {
      digitalWrite(RELAY_P, HIGH);
    } else {
      digitalWrite(RELAY_P, LOW);
    }
    prevCycleTime = time;
    prevError = error;

  }
}

void setup() {

  Serial.begin(115200);
  delay(1000);

  pinMode(SW_UP, INPUT_PULLUP);
  pinMode(SW_DOWN, INPUT_PULLUP);
  pinMode(SW_NEXT, INPUT_PULLUP);

  pinMode(RELAY_P, OUTPUT);
  pinMode(RELAY_N, OUTPUT);

  // pinMode(MOTOR_EN, OUTPUT);
  // pinMode(MOTOR_A, OUTPUT);
  // pinMode(MOTOR_B, OUTPUT);

  digitalWrite(RELAY_P, LOW);
  digitalWrite(RELAY_N, LOW);

  delay(500);

  attachInterrupt(digitalPinToInterrupt(SW_UP), incrementValue, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW_DOWN), decrementValue, CHANGE);

  display.begin();
  display.print("Ready.");

  delay(500);

  gui.update(HEATING_PARAMETERS[gui.screen]);
  //motorSpeed(0);

}

void loop() {
  bool state = digitalRead(SW_NEXT);
  time = millis();

  if (time - prevButtonCheckTime >= 10) {
    if (check_gui) {
      if (!state && STATE_SW_NEXT) {
        if (!hasAdvanced) {
          nextValue();
          hasAdvanced = true;
        }
      } else if (state && !STATE_SW_NEXT) {
        hasAdvanced = false;
      }
    } else if (!state && STATE_SW_NEXT) {
      // reset when next button is pressed while heating is going on
      // must hold for at least a second to stop
      delay(1000);
      if (!digitalRead(SW_NEXT)) {
        reset();
      }
    }

    STATE_SW_NEXT = state;
    prevButtonCheckTime = time;
  }

  time = millis();

  if (HEATING) {
    if (time % 250 <= 25) {
      temp = thermocouple.readData();
    }
    if (time % 1000 <= 25) {
      if (time % 2000 <= 25) {
        gui.printTemperatureWhileHeating(temp, phase/2);
      } else {
        gui.printHeating(phase/2);
      }
    }
    updateHeating();
  }
}