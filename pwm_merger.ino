#define DEBUG true
#define Serial if(DEBUG)Serial

const unsigned long PWM_TIMEOUT = 10000; // 10 millisecond (100 Hz) 
const byte PWM_IN[] = { 9, 10, 11 };
const byte PWM_out[] = { 3, 5, 6 };

const byte HYSTERESIS_UP = 8;
byte HYSTERESIS_UP_COUNT = 0;
const byte HYSTERESIS_DOWN = 16;
byte HYSTERESIS_DOWN_COUNT = 0;
byte CURRENT_SPEED = 0;

void setup()
{
  Serial.begin(9600);
  for (int i = 0; i < sizeof PWM_IN; i++) {
    pinMode(PWM_IN[i], INPUT);
  }
  for (int i = 0; i < sizeof PWM_out; i++) {
    pinMode(PWM_out[i], OUTPUT);
  }
}

void loop()
{
  Serial.println("loop");

  byte input_speed_max = 0;
  for (int i = 0; i < sizeof PWM_IN; i++) {
    byte speed = readPWM(PWM_IN[i]);
    Serial.print("Speed (PIN");
    Serial.print(PWM_IN[i]);
    Serial.print("): ");
    Serial.println(speed);
    if (speed > input_speed_max) {
      input_speed_max = speed;
    }
  }
  Serial.print("input_speed_max: ");
  Serial.println(input_speed_max);

  if (HYSTERESIS_UP <= 0 && HYSTERESIS_DOWN <= 0) {
    applyPWM(input_speed_max);
  }

  if (HYSTERESIS_UP > 0) {
    if (input_speed_max > CURRENT_SPEED) {
      HYSTERESIS_UP_COUNT++;
    } else {
      HYSTERESIS_UP_COUNT = 0;
    }
    if (HYSTERESIS_UP_COUNT >= HYSTERESIS_UP) {
      Serial.println("HYSTERESIS_UP_COUNT reached");
      applyPWM(input_speed_max);
      HYSTERESIS_UP_COUNT = 0;
    }
  }

  if (HYSTERESIS_DOWN > 0) {
    if (input_speed_max < CURRENT_SPEED) {
      HYSTERESIS_DOWN_COUNT++;
    } else {
      HYSTERESIS_DOWN_COUNT = 0;
    }
    if (HYSTERESIS_DOWN_COUNT >= HYSTERESIS_DOWN) {
      Serial.println("HYSTERESIS_DOWN_COUNT reached");
      applyPWM(input_speed_max);
      HYSTERESIS_DOWN_COUNT = 0;
    }
  }

  delay(1000 - (sizeof PWM_IN * 2 * (PWM_TIMEOUT / 1000))); // 1000ms minus timeout for each read
}

byte readPWM(byte pin) {
  unsigned long highDuration = pulseIn(pin, HIGH, PWM_TIMEOUT); 
  unsigned long lowDuration = pulseIn(pin, LOW, PWM_TIMEOUT);

  if (highDuration == 0 || lowDuration == 0) {
    return digitalRead(pin) == HIGH ? 255 : 0;
  }

  return (highDuration * 255) / (highDuration + lowDuration);
}

void applyPWM(byte speed) {
  Serial.print("applyPWM: ");
  Serial.println(speed);
  CURRENT_SPEED = speed;
  for (int i = 0; i < sizeof PWM_out; i++) {
    analogWrite(PWM_out[i], speed);
  }
}