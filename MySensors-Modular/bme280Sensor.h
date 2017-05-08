#include <SparkFunBME280.h>
#include <Wire.h>
#include <SPI.h>

#define SENSOR_BME280

#define MS_CHILD_TEMP 1
#define MS_CHILD_HUM 2
#define MS_CHILD_BARO 7

BME280 bme280;

MyMessage msgTemp(MS_CHILD_TEMP, V_TEMP);
MyMessage msgHum(MS_CHILD_HUM, V_HUM);
MyMessage msgPressure(MS_CHILD_BARO, V_PRESSURE);

void bme280_setup(){
  #ifdef MY_DEBUG
    Serial.println("##### SETUP # BME280 SENSOR #####");
  #endif
  
  
  
  //***Driver settings********************************//
  //commInterface can be I2C_MODE or SPI_MODE
  //specify chipSelectPin using arduino pin names
  //specify I2C address.  Can be 0x77(default) or 0x76
  
  //For I2C, enable the following and disable the SPI section
  bme280.settings.commInterface = I2C_MODE;
  //bme280.settings.I2CAddress = 0x76;
  bme280.settings.I2CAddress = BME280_I2C_ADDRESS;
  
  //For SPI enable the following and dissable the I2C section
  //bme280.settings.commInterface = SPI_MODE;
  //bme280.settings.chipSelectPin = 10;


  //***Operation settings*****************************//
  
  //runMode can be:
  //  0, Sleep mode
  //  1 or 2, Forced mode
  //  3, Normal mode
  bme280.settings.runMode = 3; //Normal mode
  
  //tStandby can be:
  //  0, 0.5ms
  //  1, 62.5ms
  //  2, 125ms
  //  3, 250ms
  //  4, 500ms
  //  5, 1000ms
  //  6, 10ms
  //  7, 20ms
  bme280.settings.tStandby = 0;
  
  //filter can be off or number of FIR coefficients to use:
  //  0, filter off
  //  1, coefficients = 2
  //  2, coefficients = 4
  //  3, coefficients = 8
  //  4, coefficients = 16
  bme280.settings.filter = 0;
  
  //tempOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  bme280.settings.tempOverSample = 1;

  //pressOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  bme280.settings.pressOverSample = 1;

  //humidOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  bme280.settings.humidOverSample = 1;

  //Calling .begin() causes the settings to be loaded
  delay(10);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.

  bme280.begin();
  
}

void bme280_presentation(){
  #ifdef MY_DEBUG
    Serial.println("##### PRESENTATION # BME280 SENSOR #####");
  #endif
  present(MS_CHILD_TEMP, S_TEMP, String(String(LOCATION_DESCRIPTION) + " Temp").c_str());
  present(MS_CHILD_HUM, S_HUM, String(String(LOCATION_DESCRIPTION) + " Humidity").c_str());
  present(MS_CHILD_BARO, S_BARO, String(String(LOCATION_DESCRIPTION) + " Baro").c_str());
}

void bme280_report(){
  if(metric)
    send(msgTemp.set(bme280.readTempC(), 2));
  else
    send(msgTemp.set(bme280.readTempF(), 2));
  send(msgHum.set(bme280.readFloatHumidity(), 2));
  float pressure = bme280.readFloatPressure() / 100;
  send(msgPressure.set(pressure, 3));

  #ifdef MY_DEBUG
    Serial.print("Temperature: ");
    Serial.print(bme280.readTempC(), 2);
    Serial.println(" degrees C");
  
    Serial.print("Temperature: ");
    Serial.print(bme280.readTempF(), 2);
    Serial.println(" degrees F");
  
    Serial.print("Pressure: ");
    Serial.print(pressure, 4);
    Serial.println(" hPa");
  
    Serial.print("Altitude: ");
    Serial.print(bme280.readFloatAltitudeMeters(), 2);
    Serial.println("m");
  
    Serial.print("Altitude: ");
    Serial.print(bme280.readFloatAltitudeFeet(), 2);
    Serial.println("ft"); 
  
    Serial.print("%RH: ");
    Serial.print(bme280.readFloatHumidity(), 2);
    Serial.println(" %");
    
    Serial.println();
  #endif
}

