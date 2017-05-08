#define RADIOBRIDGE

#include "common.h"

#define CHILD_ID_STARTINDEX 180
#define SWITCHES_OFFSET (CHILD_ID_STARTINDEX)

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();
bool mySwitch_init = false;


struct OutletSwitch{
  const MyMessage &msg;
  uint32_t pulseOn;
  uint32_t pulseOff;
};


//Child id must be incremental: used as array index.
uint8_t switchesIndex = CHILD_ID_STARTINDEX;
OutletSwitch switches[] = {
    {MyMessage(switchesIndex++, V_STATUS), 4461875, 4461884},
    {MyMessage(switchesIndex++, V_STATUS), 4462019, 4462028},
    {MyMessage(switchesIndex++, V_STATUS), 4462339, 4462348},
    {MyMessage(switchesIndex++, V_STATUS), 4463875, 4463884},
    {MyMessage(switchesIndex++, V_STATUS), 4470019, 4470028}
    };

void radioBridge_setup()
{
  #ifdef MY_DEBUG
    Serial.println("##### SETUP # RADIO BRIDGE #####");
  #endif

  mySwitch_init = true;
  mySwitch.enableTransmit(RADIO433_PIN_DATA_OUTPUT);
  mySwitch.setPulseLength(179);
  mySwitch.setRepeatTransmit(15);
  
  // Request last servo state at startup
  for(int a = 0; a < ARRAY_SIZE(switches); a++){
    #ifdef MY_DEBUG
      Serial.print("Request: ");
      Serial.println(a);
    #endif
    request(switches[a].msg.sensor, V_STATUS);
    //Wait so messages don't get lost
    wait(100);
    //send( switches[a].msg.set(0) );
  }
}

void radioBridge_presentation(){
  #ifdef MY_DEBUG
    Serial.println("##### PRESENTATION # RADIO BRIDGE #####");
  #endif

  // Register all sensors to gw
  for(int a = 0; a < ARRAY_SIZE(switches); a++){
    String s = "sw";
    s += a;
    present(switches[a].msg.sensor, S_BINARY, s.c_str(), true);
    wait(100);
  }
}

void sendPulse(uint32_t p){
  wait(100);
  #ifdef MY_DEBUG
    Serial.print("Pulse: ");
    Serial.println(p);
    if(!mySwitch_init)
      Serial.println("mySwitch not init!");
  #endif
  if(mySwitch_init)
    mySwitch.send(p, 24);
  wait(100);
}

void radioBridge_receive(const MyMessage &message){
    //Check for V_PERCENTAGE as well: something odd about HA
    if(message.type == V_STATUS || message.type == V_PERCENTAGE){
      //send( switches[a].msg.set(0) );
      if(message.sensor - SWITCHES_OFFSET >= ARRAY_SIZE(switches) || message.sensor - SWITCHES_OFFSET < 0){
        #ifdef MY_DEBUG
          Serial.print("Invalid sensor: ");
          Serial.println(message.sensor);
        #endif
        return;
      }
      int state = message.getInt();
      if(state)
        sendPulse(switches[message.sensor - SWITCHES_OFFSET].pulseOn);
      else
        sendPulse(switches[message.sensor - SWITCHES_OFFSET].pulseOff);
      #ifdef MY_DEBUG
        Serial.print("new state: ");
        Serial.print(state);
        Serial.print(" for sensor ");
        Serial.println(message.sensor - SWITCHES_OFFSET);
      #endif
      send( switches[message.sensor - SWITCHES_OFFSET].msg.set(state) );
    }
}
