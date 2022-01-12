
#include "Arduino.h"

// libs potentiometer
#include "Potentiometer.h"

// libs i2c display
#include <LiquidCrystal_I2C.h>

// libs temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin Definitions
#define POTENTIOMETER_3V3_PIN_SIG	14

// Global variables and defines
int lcdColumns = 16;
int lcdRows = 2;
int lastValueSelected = 0;
int lastValueMeasured = 0;
unsigned long lastTimeRelayTriggered;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
const int oneWireBus = 26;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
Potentiometer potentiometer_3v3(POTENTIOMETER_3V3_PIN_SIG);

void setup() 
{
    pinMode(5, OUTPUT);
    digitalWrite(5, LOW);
    lastTimeRelayTriggered = millis();
  
    // initialize LCD
    lcd.init();
    // turn on LCD backlight                      
    lcd.backlight();
  
    // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.begin(115200);

    lcd.setCursor(0,0);
    lcd.print("Goblingift");
    lcd.setCursor(0,1);
    lcd.print("Foot-Warmer");
    
    delay(5000);
    
    Serial.println("Application started!");
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void loop() 
{
    
    int selectedTemperature = readSelectedTemperature();
    Serial.print(F("Selected Temperature: ")); Serial.println(selectedTemperature);

    int measuredTemperature = readTemperature();
    Serial.print(F("Measured Temperature: ")); Serial.println(measuredTemperature);

    printValuesToDisplay(selectedTemperature, measuredTemperature);

    // make sure relay gets triggered only every 10 seconds
    Serial.println(millis());
    Serial.println(lastTimeRelayTriggered);
    
    if (millis() - lastTimeRelayTriggered >= 10000) {
      if (measuredTemperature >= selectedTemperature) {
        triggerRelayHeater(false);
      } else {
        triggerRelayHeater(true);
      }
    }
}

void triggerRelayHeater(boolean activate) {
  Serial.println("called triggerRelayHeater");
  Serial.println(activate);
  
  if (activate) {
    Serial.println("power on!");
    digitalWrite(5, HIGH);
  } else {
    Serial.println("power off!");
    digitalWrite(5, LOW);
  }

  lastTimeRelayTriggered = millis();
}

float readTemperature() {
    sensors.requestTemperatures();
    float temperatureC = sensors.getTempCByIndex(0);
    int convertedTemperatureC = temperatureC; 
  
    return convertedTemperatureC;
}

void printValuesToDisplay(int selectedTemperature, int measuredTemperature) {

    if (lastValueSelected != selectedTemperature || lastValueMeasured != measuredTemperature) {
      String selectedTemperatureString = String(selectedTemperature);
      String measuredTemperatureString = String(measuredTemperature);
  
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Actual    Target");
      
      lcd.setCursor(0,1);
      lcd.print(measuredTemperatureString);
      lcd.setCursor(5,1);
      lcd.print("Celsius");
      lcd.setCursor(14,1);
      lcd.print(selectedTemperatureString);
    }

    lastValueSelected = selectedTemperature;
    lastValueMeasured = measuredTemperature;
}

int readSelectedTemperature() {

    int temperatures[10];
    int i,j;
    int result = 0;
    
    for (i=0; i < 10; i++) {
      int potentiometer_3v3Val = potentiometer_3v3.read();
      int selected_temperature = ((4095 - potentiometer_3v3Val) / 136.5) + 20;

      temperatures[i] = selected_temperature;

      result = 0;
      for (j = 0; j < 10; j++) {
        result = result + temperatures[j]; // add them up
      }
      result = result / 10;
      
      delay(5);
    }

  return result;
}
