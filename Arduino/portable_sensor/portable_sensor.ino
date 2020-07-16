#include <ArduinoBLE.h>
#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>

BLEService WeatherStation("c86d00ac-5805-4fbf-be7c-66d031d4360e");
BLEFloatCharacteristic temperature("2A1F", BLERead);


float oldTemperature = 0;  // last temperature reading
long previousMillis = 0;  // last time the temperature was checked, in ms

void setup() {
  Serial.begin(9600);    // initialize serial communication
  while (!Serial);

  if (!BARO.begin()) {
    Serial.println("Failed to initialize pressure sensor!");
    while (1);
  }

  if (!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while (1);
  }
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }


  BLE.setLocalName("Weather station");
  BLE.setAdvertisedService(WeatherStation);
  WeatherStation.addCharacteristic(temperature);
  BLE.addService(WeatherStation);
  temperature.writeValue(oldTemperature);

  //BLE.setConnectable(false);

  BLE.advertise();
  Serial.println("Success!");
}

void loop()
{
  BLEDevice central = BLE.central();
  if (central)
  {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());

    while (central.connected())
    {
      long currentMillis = millis();
      if (currentMillis - previousMillis >= 10000)
      {
        previousMillis = currentMillis;
        updateTemperature();
      }
    }
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

void updateTemperature()
{
  float currentTemperature = HTS.readTemperature();

  if (oldTemperature != currentTemperature)
  {
    temperature.writeValue(currentTemperature);
    oldTemperature = currentTemperature;
    Serial.print("Current temperature: ");
    Serial.print(currentTemperature);
    Serial.println(" °C");
  }
}
