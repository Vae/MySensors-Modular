#define SENSOR_ANALOGLIGHT

#define CHILD_ID_LIGHTANALOG 4

int analoglight_sensor_last = 0;
#define ANALOGLIGHT_TOLERANCE 20

MyMessage msgLightAnalog(CHILD_ID_LIGHTANALOG, V_LIGHT_LEVEL);

void light_setup(){
  #ifdef MY_DEBUG
    Serial.println("### SETUP # ANALOG LIGHT SENSOR #####");
  #endif
  //pinMode(ANALOGLIGHT_PIN_DATA_INPUT, INPUT);      // sets the motion sensor digital pin as input
}

void light_presentation(){
  #ifdef MY_DEBUG
    Serial.println("##### PRESENTATION # ANALOG LIGHT SENSOR #####");
  #endif
  present(CHILD_ID_LIGHTANALOG, S_LIGHT_LEVEL, LOCATION_DESCRIPTION);
}

int light_ReadValue(){
  return 1023 - analogRead(ANALOGLIGHT_PIN_DATA_INPUT) / 2;
}

void light_report(){
  #ifdef MY_DEBUG
    Serial.print(1023 - analogRead(ANALOGLIGHT_PIN_DATA_INPUT) / 2);
    Serial.println(" lux");
  #endif
  int current = light_ReadValue();
  send(msgLightAnalog.set(current));
  analoglight_sensor_last = current;
}

void light_check(){
  int current = light_ReadValue();
  if(abs(current - analoglight_sensor_last) > ANALOGLIGHT_TOLERANCE){
    light_report();
  }
}


