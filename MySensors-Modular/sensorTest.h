#define SENSOR_TEST

#define SENSOR_TEST_MAX_VALUE 1200

int sensor_test_value = 0;
#define CHILD_ID_SENSORTEST 0

MyMessage msgLightAnalog(CHILD_ID_SENSORTEST, V_LIGHT_LEVEL);

void testSensor_presentation(){
  #ifdef MY_DEBUG
    Serial.println("##### PRESENTATION # SENSOR TEST #####");
  #endif
  //present(CHILD_ID_SENSORTEST, S_LIGHT_LEVEL, LOCATION_DESCRIPTION);
}

void testSensor_setup(){
  #ifdef MY_DEBUG
    Serial.println("##### SETUP # SENSOR TEST #####");
  #endif
}

void testSensor_check(){
  /*sensor_test_value = sensor_test_value + 1;
  if(sensor_test_value >= SENSOR_TEST_MAX_VALUE)
    sensor_test_value = 0;
  send(msgLightAnalog.set(sensor_test_value));*/
}

