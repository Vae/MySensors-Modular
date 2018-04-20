#define TESTING_SENSOR

// Known issues:
//  * Hass doesn't seem to line up the GUI with the sensor capabilities. Turning off the main switch disables the RGB component.
//


#define SENSOR_TEST_MAX_VALUE 1200

#define CHILD_ID_SENSORTEST 0
#define CHILD_ID_SENSORTEST1 1
#define CHILD_ID_SENSORTEST2 2

MyMessage msgRGB(CHILD_ID_SENSORTEST, V_RGBW);
MyMessage msgDimmer(CHILD_ID_SENSORTEST1, V_DIMMER);
MyMessage msgStatus(CHILD_ID_SENSORTEST2, V_STATUS);

uint8_t r = 0, g = 0, b = 0;

bool Testing_init[] = {false, false, false};

void Testing_presentation(){
  #ifdef MY_DEBUG
    Serial.println("##### PRESENTATION # SENSOR TEST #####");
  #endif
  present(CHILD_ID_SENSORTEST, S_RGBW_LIGHT, "RGBW", true);
  present(CHILD_ID_SENSORTEST1, S_DIMMER);
  present(CHILD_ID_SENSORTEST2, S_BINARY);
}



void Testing_setup(){
  #ifdef MY_DEBUG
    Serial.println("##### SETUP # SENSOR TEST #####");
  #endif

  Serial.println(":::");
  Serial.println(msgRGB.sensor);
  Serial.println(msgDimmer.sensor);
  Serial.println(msgStatus.sensor);
  
  //request(msgRGB.sensor, V_RGB);
  wait(500);
//  if(!Testing_init[msgRGB.sensor])
    send(msgRGB.set("ff0000ff"));
    
//  request(msgDimmer.sensor, V_DIMMER);
//  wait(500);
//  if(!Testing_init[msgDimmer.sensor])
    send(msgDimmer.set(0));
    
//  request(msgStatus.sensor, V_STATUS);
//  wait(500);
//  if(!Testing_init[msgStatus.sensor])
    send(msgStatus.set(0));
}

void Testing_check(){
  /*sensor_test_value = sensor_test_value + 1;
  if(sensor_test_value >= SENSOR_TEST_MAX_VALUE)
    sensor_test_value = 0;
  send(msgLightAnalog.set(sensor_test_value));*/
}

void Testing_receive(const MyMessage &message){

  // DEBUG
  Serial.print("Got a message - ");
  Serial.print("Messagetype is: ");
  Serial.print(message.type);
  Serial.print(":");
  Serial.println(message.sensor);
  
  // END DEBUG

  // Acknoledgment
  if (message.isAck()) {
    
    Serial.println("Got ack from gateway");
    
  } else if (message.type == V_DIMMER) {
    Testing_init[message.sensor] = true;
  
    Serial.println("Dimming to ");
    Serial.println(message.getInt());
    send(msgDimmer.set(message.getInt()));

    // Do dimmer stuff here
      
  } else if (message.type == V_STATUS) {
    Testing_init[message.sensor] = true;
    Serial.print("Turning light ");
    Serial.println(message.getInt());
    send(msgStatus.set(message.getInt()));

    // Do On/Off stuff here

  } else if (message.type == V_RGBW) {
    Testing_init[message.sensor] = true;
    send(msgRGB.set(message.getString()));
    long number;
    //Grab the White value:
    number = strtol( message.getString() + 6, NULL, 16);
    int w = number & 0xFF;
    char buf[7];
    memcpy( buf, message.getString(), 6);
    buf[6] = NULL;
    number = strtol( buf, NULL, 16);
    Serial.println("status:" );
    Serial.println(buf);
    Serial.println(message.getString() + 6);
    Serial.println(number);
    r = number >> 16 & 0xFF;
    g = number >> 8 & 0xFF;
    b = number & 0xFF;
    analogWrite(A1, b);
    analogWrite(A2, g);
    analogWrite(A3, r);
    Serial.print("Values: ");
    Serial.print(r);
    Serial.print(", ");
    Serial.print(g);
    Serial.print(", ");
    Serial.print(b);
    Serial.print(", ");
    Serial.print(w);
    Serial.print("RGBW!: ");
    Serial.println(message.getString());
        
    Serial.println("+++");
    // Set color here
  }  
}

