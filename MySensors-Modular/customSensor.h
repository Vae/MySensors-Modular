#define CUSTOMSENSOR
#include <string.h>
struct Message{
  const MyMessage &msg;
  bool init;
};


#define CUSTOMSENSOR_ID 0
#define CUSTOMSENSOR_ID_VAL 1
#define CUSTOMSENSOR_ID_TRIG 2
#define CUSTOMSENSOR_DESCRIPTION "zxzxzx"
#define CUSTOMSENSOR_DESCRIPTION_VAL "jjjj"

int start = 0;


Message ledLight_msgRGBW {MyMessage ( CUSTOMSENSOR_ID, V_TEXT), false};
//Message myText {MyMessage ( CUSTOMSENSOR_ID_VAL, V_TEXT), false};
Message trigger {MyMessage ( CUSTOMSENSOR_ID_TRIG, V_STATUS), false};

void customSensor_setup(){
  #ifdef MY_DEBUG
    Serial.println("##### SETUP # CUSTOM SENSOR #####");
  #endif
  request(CUSTOMSENSOR_ID, V_TEXT);
  send( trigger.msg.set(0) );
}

void customSensor_doInit(){
  //send( ledLight_msgRGBW.msg.set("TEST") );
}

void customSensor_check(){
  start++;
  if(start%8==0){
    
    //request(CUSTOMSENSOR_ID, V_TEXT);
    //wait(2000, C_SET, V_TEXT);
    String s("XXXTESTXXX: ");
    s += start;
    Serial.println("Send " + s);
    //send( ledLight_msgRGBW.msg.set(s.c_str()) );
    request(CUSTOMSENSOR_ID, V_TEXT);
    wait(200);
    
    send( trigger.msg.set(0) );
  }
}

void customSensor_presentation(){
  #ifdef MY_DEBUG
    Serial.println("##### PRESENTATION # CUSTOM SENSOR #####");
  #endif
  present(CUSTOMSENSOR_ID, S_INFO , CUSTOMSENSOR_DESCRIPTION, true);
//  present(CUSTOMSENSOR_ID_VAL, S_CUSTOM , CUSTOMSENSOR_DESCRIPTION, true);
  present(CUSTOMSENSOR_ID_TRIG, S_BINARY, "Xend", true);
}

void customSensor_receive(const MyMessage &message){
  wait(100);
  if (message.isAck()) {
    #ifdef MY_DEBUG
      Serial.println("ACK");
    #endif
  } else if (message.type == V_TEXT) {
    #ifdef MY_DEBUG
      Serial.print("Got message ");
      Serial.println(message.getString());
    #endif
  }
  else {
    #ifdef MY_DEBUG
      Serial.print("Unused message of type ");
      Serial.println(message.type);
    #endif
  }
}

