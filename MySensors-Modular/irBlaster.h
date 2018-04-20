#define SENSOR_IRBLASTER

#include "common.h"

//The IRBlaster must be on pin 3

#define CHILD_ID_IRBLASTER 9
#define CHILD_ID_IRBLASTER_STARTINDEX 150
#define IRBLASTER_OFFSET (CHILD_ID_IRBLASTER_STARTINDEX)
#include <IRremote.h>  // https://github.com/z3t0/Arduino-IRremote/releases

struct Code
{
  decode_type_t type;            // The type of code
  unsigned long value;           // The data bits if type is not raw
  int           len;             // The length of the code in bits
  unsigned int  address;         // Used by Panasonic & Sharp [16-bits]
};

struct IRCode
{
  const MyMessage &msg;
  Code code;
};

#define MAX_STORED_IR_CODES 5

IRsend irSend;
bool irSend_init = false;
uint32_t irIndex = CHILD_ID_IRBLASTER_STARTINDEX;
IRCode irCodes[] = {
      { MyMessage(irIndex++, V_STATUS), { NEC, 0x2FD48B7,   32, 0 }}, // Power
      { MyMessage(irIndex++, V_STATUS), { NEC, 0x2FD58A7,   32, 0 }}, // Vol Up
      { MyMessage(irIndex++, V_STATUS), { NEC, 0x2FD7887,   32, 0 }}, // Vol Down
      { MyMessage(irIndex++, V_STATUS), { NEC, 0x2FD08F7,   32, 0 }}, // Mute
      { MyMessage(irIndex++, V_STATUS), { NEC, 0x2FDA857,   32, 0 }}, // Sleep
      { MyMessage(irIndex++, V_STATUS), { NEC, 0x2FDF00F,   32, 0 }}  // Input
  };

void irBlaster_presentation(){
  for(int a = 0; a < ARRAY_SIZE(irCodes); a++){
    String s = "ir";
    s += a;
    present(irCodes[a].msg.sensor, S_BINARY, s.c_str(), true);
    wait(100);
  }
}

void irBlaster_setup(){
  irSend_init = true;
}

void irBlaster_send(Code &dis){
  #ifdef MY_DEBUG
    Serial.print("Blast: ");
    Serial.println(dis.value);
    if(!irSend_init)
      Serial.println("irSend not init!");
  #endif
  if(irSend_init){
//    for(int a = 0; a < 3; a++){
      irSend.sendNEC(dis.value, dis.len);
//      wait(40);
//    }
  }
}

void irBlaster_receive(const MyMessage &message){
  //Check for V_PERCENTAGE as well: something odd about HA
  if(message.type == V_STATUS || message.type == V_PERCENTAGE){
    
    if(message.sensor - IRBLASTER_OFFSET >= ARRAY_SIZE(irCodes) || message.sensor - IRBLASTER_OFFSET < 0){
      #ifdef MY_DEBUG
        Serial.print("Invalid code: ");
        Serial.println(message.sensor);
      #endif
      return;
    }
    irBlaster_send(irCodes[message.sensor - IRBLASTER_OFFSET].code);
    #ifdef MY_DEBUG
      Serial.print("Trigger for sensor ");
      Serial.println(message.sensor - IRBLASTER_OFFSET);
    #endif
    send( irCodes[message.sensor - IRBLASTER_OFFSET].msg.set(0) );
  }
}

