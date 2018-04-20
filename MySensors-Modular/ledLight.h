#define RGBW_ADDRESSABLE_STRIP

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_PIN_DATA 3
#define LED_COUNT 300
#define LED_MAX_BRIGHTNESS 230

//Starting brightness
uint8_t LED_Brightness = 50;

struct Message{
  const MyMessage &msg;
  bool init;
};


#define LED_LIGHT_SENSOR_ID 0
#define LED_LIGHT_DESCRIPTION "Addressable LED Strip"

Message ledLight_msgRGBW {MyMessage ( LED_LIGHT_SENSOR_ID, V_RGBW   ), false};
Message ledLight_msgStatus {MyMessage ( LED_LIGHT_SENSOR_ID, V_STATUS   ), false};
Message ledLight_msgDimmer {MyMessage ( LED_LIGHT_SENSOR_ID, V_DIMMER   ), false};

//MyMessage ledLight_msgRGBW   ( LED_LIGHT_SENSOR_ID, V_RGBW   );
//MyMessage ledLight_msgStatus ( LED_LIGHT_SENSOR_ID, V_STATUS );
//MyMessage ledLight_msgDimmer ( LED_LIGHT_SENSOR_ID, V_DIMMER );
bool ledLight_init = false;
uint8_t red = 0, green = 0, blue = 0, white = 0;
uint8_t mode = 0;
//NEO_GRBW
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN_DATA, NEO_RGBW + NEO_KHZ800);

void rainbow(uint8_t);
void colorSet(uint32_t);

void ledLight_setup(){
  #ifdef MY_DEBUG
    Serial.println("##### SETUP # LED LIGHT STRIP #####");
  #endif
  
  strip.begin();
  //strip.setBrightness(LED_Brightness);
  //strip.show();
  
  request(LED_LIGHT_SENSOR_ID, V_RGBW);
  wait(200);
  request(LED_LIGHT_SENSOR_ID, V_STATUS);
  wait(200);
  request(LED_LIGHT_SENSOR_ID, V_DIMMER);
  wait(200);
  

  //strip.show(); // Initialize all pixels to 'off'

  delay(100);
}
void ledLight_doInit(){
  #ifdef MY_DEBUG
    Serial.println("Send init");
  #endif
  send( ledLight_msgRGBW.msg.set("ffffffff") );
  send( ledLight_msgDimmer.msg.set(LED_Brightness) );
  send( ledLight_msgStatus.msg.set(mode) );
  ledLight_init = true;
}

void ledLight_loop(){
  if(ledLight_init == false){
    ledLight_doInit();
  }
  else{
    switch(mode){
      case 2: rainbow(50); break;
      
    }
    
  }
}

void ledLight_presentation(){
  
  present(LED_LIGHT_SENSOR_ID, S_RGBW_LIGHT, LED_LIGHT_DESCRIPTION, true);
  // get old values if this is just a restart
  //request(LED_LIGHT_SENSOR_ID, V_RGBW);
}

void ledLight_receive(const MyMessage &message){

  // DEBUG
  #ifdef MY_DEBUG
    Serial.print("Message.type: ");
    Serial.print(message.type);
    Serial.print(": ");
    Serial.println(message.sensor);
  #endif
  wait(100);
  
  if (message.isAck()) {
    #ifdef MY_DEBUG
      Serial.println("ACK");
    #endif
  } else if (message.type == V_DIMMER) {
    send(ledLight_msgDimmer.msg.set(message.getInt()));
    LED_Brightness = message.getFloat() / 100 * LED_MAX_BRIGHTNESS;
  } else if (message.type == V_STATUS) {
    send(ledLight_msgStatus.msg.set(message.getInt()));
    //If turning the light on, ensure that brightness isn't set to zero. Set it to LED_MAX_BRIGHTNESS/4
    if(message.getInt() > 0 && LED_Brightness < 1){
      LED_Brightness = LED_MAX_BRIGHTNESS >> 2;
      wait(100);
      send(ledLight_msgDimmer.msg.set(LED_Brightness));
    }
    //If turning the light on, ensure that RGBW isn't set to zero. Set it to LED_MAX_BRIGHTNESS/4
    if(message.getInt() > 0 && red < 1 && green < 1 && blue < 1){
      red = 0xff;
      green = 0xff;
      blue = 0xff;
      white = 0xff;
      wait(100);
      send(ledLight_msgRGBW.msg.set("ffffffff"));
    }
    
    mode = message.getInt();
    
  } else if (message.type == V_RGBW) {
    ledLight_init = true;
    #ifdef MY_DEBUG
      Serial.print("RGBW:");
      Serial.println(message.getString());
    #endif
    send(ledLight_msgRGBW.msg.set(message.getString()));
    uint64_t num = strtoul( message.getString(), NULL, 16);
    red = num >> 24;
    green = num >> 16 & 0xff;
    blue = num >> 8 & 0xff;
    white = num & 0xff;
  }

  #ifdef MY_DEBUG
    Serial.print("Mode: ");
    Serial.print(mode);
    Serial.print(":");
    Serial.print(LED_Brightness);
    Serial.print(" red:");
    Serial.print(red);
    Serial.print(" green:");
    Serial.print(green);
    Serial.print(" blue:");
    Serial.print(blue);
    Serial.print(" white:");
    Serial.print(white);
    Serial.println();
  #endif

  if(mode > 0){
    //some "on" state
    if(red > 250 && green > 250 && blue > 250)
      colorSet(strip.Color(0, 0, 0, white));
    else
      colorSet(strip.Color(red, green, blue, white));
  }
  else  //off
    colorSet(strip.Color(0, 0, 0, 0));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Fill the dots one after the other with a color
void colorSet(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if(i%2 == 0)
      strip.setPixelColor(i, c);
    else
      strip.setPixelColor(i, 0);
  }
  strip.setBrightness(LED_Brightness);
  strip.show();
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3,0);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3,0);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0,0);
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256 * 5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}



void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void fullWhite() {
  
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0,0,0, 255 ) );
    }
      strip.show();
}
