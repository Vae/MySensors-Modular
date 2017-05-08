#include <DHT.h>

#define SENSOR_DHT

#ifndef DHT_TEMP_OFFSET
#define DHT_TEMP_OFFSET 0
#endif

DHT dht;

// ---------------------- MySensors
#define MS_CHILD_TEMP 1
#define MS_CHILD_HUM 2

MyMessage msgTemp(MS_CHILD_TEMP, V_TEMP);
MyMessage msgHum(MS_CHILD_HUM, V_HUM);

void temphum_setup(){
  #ifdef MY_DEBUG
    Serial.println("##### SETUP # DHT11 SENSOR #####");
  #endif
  dht.setup(DHT_PIN_DATA_INPUT); // set data pin of DHT sensor
  wait(2000);
}

void temphum_presentation(){
  #ifdef MY_DEBUG
    Serial.println("##### PRESENTATION # DHT11 SENSOR #####");
  #endif
  //String s = LOCATION_DESCRIPTION
  present(MS_CHILD_TEMP, S_TEMP, LOCATION_DESCRIPTION);
  present(MS_CHILD_HUM, S_HUM, LOCATION_DESCRIPTION);
}

void temphum_report(){
    #ifdef DEBUG
      Serial.println("Temp/Hum read");
    #endif

  // Get temperature from DHT
  float temperature = dht.getTemperature();
  if (isnan(temperature)) {
    Serial.println("Failed reading temperature from DHT!");
  }
  else{
    if (!metric) {
      temperature = dht.toFahrenheit(temperature);
    }
    temperature += DHT_TEMP_OFFSET;
    send(msgTemp.set(temperature, 1));
  }
  // Get humidity from DHT library
  float humidity = dht.getHumidity();
  if (isnan(humidity)) {
    Serial.println("Failed reading humidity from DHT");
  }
  else{
    send(msgHum.set(humidity, 1));
  }
}

