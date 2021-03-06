#define MY_DEBUG

// #define ACT_AS_GATEWAY

//Are we using a battery operated device?
//#define LOW_POWER

// Enable and select radio type attached
#define MY_RADIO_NRF24

//Basement: 0
//Main: 1
//Bedroom: 2
//LedStrip 3
//Basement laundry room 4
//Kitchen 5
//Test: 99
#define MY_NODE_ID 77
#define DEVICE_VERSION "1.2"

// --------------------- NRF24L01 2.4Ghz Radio uses pin: 13, 12, 11, 10, 9

#ifdef MY_NODE_ID == 0
  #define MY_NODE_NAME "Gateway"
  #define LOCATION_DESCRIPTION "Basement"
#endif

#ifdef MY_NODE_ID == 1
  #define MY_NODE_NAME "Main"
  #define LOCATION_DESCRIPTION "Living room"

  #define MOTION_PIN_DATA_INPUT 2
  #define MOTION_TIMEOUT 120000
  #define IRBLASTER_PIN_DATA_OUTPUT 3
  #define RADIO433_PIN_DATA_OUTPUT 6
  #define BME280_I2C_ADDRESS 0x76
#endif

#ifdef MY_NODE_ID == 2

#endif

#ifdef MY_NODE_ID == 3

#endif

#ifdef MY_NODE_ID == 4

#endif

#ifdef MY_NODE_ID == 5

#endif

#ifdef MY_NODE_ID == 6

#endif

#ifdef MY_NODE_ID == 77
  #define MY_NODE_NAME "TestNode"
  #define LOCATION_DESCRIPTION "Basement"

  #define SENSORCUSTOM_PIN 5
#endif


#ifndef MY_NODE_ID
  #error MY_NODE_ID must be defined
#endif

#ifndef MY_NODE_NAME
  #error MY_NODE_NAME must be defined
#endif

#ifndef LOCATION_DESCRIPTION
  #error LOCATION_DESCRIPTION must be defined
#endif

#ifndef DEVICE_VERSION
  #error DEVICE_VERSION must be defined
#endif


#define MY_RF24_PA_LEVEL RF24_PA_HIGH

#ifdef ACT_AS_GATEWAY
  // Set LOW transmit power level as default, if you have an amplified NRF-module and
  // power your radio separately with a good regulator you can turn up PA level.
  
  
  // Enable serial gateway
  #define MY_GATEWAY_SERIAL
  
  // Define a lower baud rate for Arduino's running on 8 MHz (Arduino Pro Mini 3.3V & SenseBender)
  #if F_CPU == 8000000L
  #define MY_BAUD_RATE 115200
  #endif
  
  // Enable inclusion mode
  #define MY_INCLUSION_MODE_FEATURE
  // Enable Inclusion mode button on gateway
  //#define MY_INCLUSION_BUTTON_FEATURE
  
  // Set inclusion mode duration (in seconds)
  #define MY_INCLUSION_MODE_DURATION 60
  
  // Set blinking period
  #define MY_DEFAULT_LED_BLINK_PERIOD 300
  
  // Flash leds on rx/tx/err
  // Uncomment to override default HW configurations
  //#define MY_DEFAULT_ERR_LED_PIN 4  // Error led pin
  //#define MY_DEFAULT_RX_LED_PIN  6  // Receive led pin
  //#define MY_DEFAULT_TX_LED_PIN  5  // the PCB, on board LED
  #define DEVICE_TYPE "Sensor Gateway"
#else
  #ifndef MY_NODE_NAME
    #define DEVICE_TYPE "Sensor Host"
  #else
    #define DEVICE_TYPE MY_NODE_NAME
  #endif
#endif

bool metric = false;

#include "common.h"
#include <MySensors.h>
#include <TaskScheduler.h>
#ifdef LOW_POWER
  #include "LowPower.h"
#endif

// ---------------------- Motion Sensor
#ifdef MOTION_PIN_DATA_INPUT
  //#include "motionSensor.h"
#endif

// --------------------- 433Mhz Transmitter
#ifdef RADIO433_PIN_DATA_OUTPUTA
  //#include "RadioBridge.h"
#endif

#ifdef DHT_PIN_DATA_INPUT
  //#include "temphumSensor.h"
#endif

// ---------------------- BME280 Sensor
#ifdef BME280_I2C_ADDRESS
  //#include "bme280Sensor.h"
#endif

//#include "lightSensor.h"

// --------------------- IR Blaster
#ifdef IRRECEIVER_PIN_DATA_INPUT
  //#include "irBlaster.h"
#endif

#ifdef SENSORCUSTOM_PIN
  #include "customSensor.h"
#endif

//#include "RGBAnalog.h"

#ifdef LED_PIN_DATA
  //#include "ledLight.h"
#endif


// ---------------------- Scheduler
Scheduler scheduler;
void read5Min();
void read300ms();

Task slowTask(300000, TASK_FOREVER, read5Min);
Task fastTask(1000, TASK_FOREVER, read300ms);

void setup()
{
  scheduler.init();
  #ifdef MY_DEBUG
    Serial.begin(115200);
    Serial.print("id: ");
    Serial.print(getNodeId());
    Serial.println();
  #endif

  #ifdef CUSTOMSENSOR
    customSensor_setup();
  #endif

  #ifdef RADIOBRIDGE
    radioBridge_setup();
  #endif

  #ifdef SENSOR_IRBLASTER
    irBlaster_setup(); 
  #endif

  #ifdef SENSOR_MOTION
    motion_setup(scheduler);
  #endif
  
  #ifdef SENSOR_DHT
    temphum_setup();
  #endif
  
  #ifdef SENSOR_BME280
    bme280_setup();
  #endif

  #ifdef SENSOR_TEST
    testSensor_setup();
  #endif
  
  #ifdef RGBW_ADDRESSABLE_STRIP
    ledLight_setup();
  #endif

  #ifdef RGBANALOG
    RGBAnalog_setup();
  #endif
  
  //Power-up delay: 3200ms
  wait(3200);

  scheduler.addTask(slowTask);
  scheduler.addTask(fastTask);
  #ifdef MY_DEBUG
    Serial.println("Exec");
  #endif
  slowTask.enable();
  fastTask.enable();
}

void presentation()
{
    // Send the sketch version information to the gateway and Controller
    sendSketchInfo(DEVICE_TYPE, DEVICE_VERSION);
    //metric = getControllerConfig().isMetric;
    
  #ifdef CUSTOMSENSOR
    customSensor_presentation();
  #endif

  #ifdef SENSOR_MOTION
    motion_presentation();
  #endif

  #ifdef SENSOR_DHT
    temphum_presentation();
  #endif

  #ifdef SENSOR_BME280
    bme280_presentation();
  #endif

  #ifdef RADIOBRIDGE
    radioBridge_presentation();
  #endif

  #ifdef SENSOR_IRBLASTER
    irBlaster_presentation();
  #endif
  
  #ifdef SENSOR_TEST
    testSensor_presentation();
  #endif

  #ifdef RGBW_ADDRESSABLE_STRIP
    ledLight_presentation();
  #endif

  #ifdef RGBANALOG
    RGBAnalog_presentation(); 
  #endif

  
}

// Sensors that are only updated on a given interval
void read5Min(){
  // Update/refresh all sensors
  
  #ifdef SENSOR_MOTION
    motion_report();
  #endif
  
  #ifdef SENSOR_DHT
    temphum_report();
  #endif

  #ifdef SENSOR_BME280
    bme280_report();
  #endif
  
  #ifdef SENSOR_ANALOGLIGHT
    light_report();
  #endif
}

void read300ms(){
  #ifdef CUSTOMSENSOR
    customSensor_check();
  #endif


  #ifdef SENSOR_MOTION
    motion_check();
  #endif

  #ifdef SENSOR_ANALOGLIGHT
    light_check();
  #endif

  #ifdef SENSOR_TEST
    testSensor_check();
  #endif
  //irSend.sendNEC(irCodes[0].code.value, irCodes[0].code.len);
}

void loop()
{
  scheduler.execute();
  wait(100);
  #ifdef LOW_POWER
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  #endif
  #ifdef RGBW_ADDRESSABLE_STRIP
    ledLight_loop();
  #endif
}


void receive(const MyMessage &message){
  #ifdef MY_DEBUG
    Serial.print("Message.type: ");
    Serial.print(message.type);
    Serial.print(": ");
    Serial.println(message.sensor);
  #endif
  #ifdef CUSTOMSENSOR
    customSensor_receive(message);
  #endif

  #ifdef RADIOBRIDGE
    radioBridge_receive(message);
  #endif

  #ifdef SENSOR_IRBLASTER
    irBlaster_receive(message);
  #endif

  #ifdef RGBW_ADDRESSABLE_STRIP
    ledLight_receive(message);
  #endif

  #ifdef RGBANALOG
    RGBAnalog_receive(message);
  #endif
}
