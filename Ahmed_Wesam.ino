#include <Ultrasonic.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define MAX_LIGHT_VALUE 10
#define MIN_LIGHT_VALUE 0
#define LIGHT_VALUE_THRESHOLD MAX_LIGHT_VALUE/2
#define MAX_ADC 1024
#define LIGHT_ADC_THRESHOLD 0

#define RIGHT_LED_PIN 8
#define LEFT_LED_PIN 9

enum LED_Status {
  ON = true,
  OFF = false
};

enum Street {
  RIGHT,
  LEFT
};

LED_Status ledStatus;
int rightLEDTime;
int leftLEDTime;
int lightValue;

/*
 * Pass as a parameter the trigger and echo pin, respectively,
 * or only the signal pin (for sensors 3 pins), like:
 * Ultrasonic ultrasonic(13);
 */
Ultrasonic rightUltrasonic(10, 11);
Ultrasonic leftUltrasonic(12, 13);

/*
 * set the LCD address to 0x27 for a 16 chars and 2 line display.
 */
LiquidCrystal_I2C lcd(0x3f, 16, 2);

void setup() {
  Serial.begin(9600);
  pinMode(RIGHT_LED_PIN, OUTPUT);
  pinMode(LEFT_LED_PIN, OUTPUT);
  lcd.init();
  lcd.backlight();
}

void loop() {
  if (ledStatus == LED_Status::OFF) {
    if (senseLight()) {
      senseVehicle();
    }
  } else
  if (ledStatus == LED_Status::ON){
    senseVehicle();
  }
}

bool senseLight() {
  int value = analogRead(A0);
  lcdPrintValue("Light: ", value, 0, 0);
  if (value > LIGHT_ADC_THRESHOLD) {
    increaseLightValue();
  } else {
    decreaseLightValue();
  }
  return lightValue < LIGHT_VALUE_THRESHOLD;
}

void senseVehicle() {
  /*
   * Reas distance in centimeters
   */
  int distance = rightUltrasonic.read();
  lcdPrintValue("D1: ", distance, 0, 1);
  Serial.println(distance);
  if (distance > 5 && distance < 25) {
    Serial.print("Distance1: ");
    Serial.println(distance);
    turnOnLights(Street::RIGHT);
  }
//  Serial.println(millis()-rightLEDTime);
  if ((millis()-rightLEDTime) > 5000) {
    turnOffLights(Street::RIGHT);
  }

  distance = leftUltrasonic.read();
  lcdPrintValue("D2: ", distance, 8, 1);
//  Serial.print("Distance2: ");
//  Serial.println(distance);
  if (distance > 5 && distance < 25) {
    turnOnLights(Street::LEFT);
  }
  if ((millis()-leftLEDTime) > 5000) {
    turnOffLights(Street::LEFT);
  }
}

void increaseLightValue() {
  if (lightValue >= MAX_LIGHT_VALUE) {
    lightValue = MAX_LIGHT_VALUE;
  } else {
    lightValue++;
  }
}

void decreaseLightValue() {
  if (lightValue == MIN_LIGHT_VALUE) {
    lightValue = MIN_LIGHT_VALUE;
  } else {
    lightValue--;
  }
}

void turnOnLights(Street street) {
  if (street == Street::RIGHT) {
    analogWrite(RIGHT_LED_PIN, 255);
  } else
  if (street == Street::LEFT) {
    analogWrite(LEFT_LED_PIN, 255);
  }
  ledStatus = LED_Status::ON;
  rightLEDTime = millis();
}

void turnOffLights(Street street) {
  Serial.println("Turn off led");
  if (street == Street::RIGHT) {
    analogWrite(RIGHT_LED_PIN, 1);
  } else
  if (street == Street::LEFT) {
    analogWrite(LEFT_LED_PIN, 1);
  }
  ledStatus = LED_Status::OFF;
  leftLEDTime = millis();
}

void lcdPrintValue(String msg, int value, int column, int row) {
  msg += value;
  msg += "   ";
  lcd.setCursor(column, row);
  lcd.print(msg.c_str());
}
