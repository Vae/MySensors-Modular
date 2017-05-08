// #define MY_DEBUG

// #define ACT_AS_GATEWAY

// Enable and select radio type attached
#define MY_RADIO_NRF24

#define LOCATION_DESCRIPTION "Main"
//Basement: 0
//Main: 1
//Bedroom: 2
//Test: 4
#define MY_NODE_ID 1

#ifdef ACT_AS_GATEWAY
  // Set LOW transmit power level as default, if you have an amplified NRF-module and
  // power your radio separately with a good regulator you can turn up PA level.
  #define MY_RF24_PA_LEVEL RF24_PA_LOW
  
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
  #define DEVICE_TYPE "Sensor Host"
#endif


// ---------------------- About me
#define DEVICE_VERSION "1.2"

// --------------------- NRF24L01 2.4Ghz Radio
// Uses pin: 13, 12, 11, 10, 9

// ---------------------- Motion Sensor
#define MOTION_PIN_DATA_INPUT 2 //(Only 2 and 3 generates interrupt)
#define MOTION_TIMEOUT 120000

// ---------------------- TempHum DHT11 Sensor
#define DHT_PIN_DATA_INPUT 5

// ---------------------- BME280 Sensor
#define BME280_I2C_ADDRESS 0x76

// ---------------------- Analog Light Sensor
#define ANALOGLIGHT_PIN_DATA_INPUT 0

// --------------------- IR Blaster
#define IRBLASTER_PIN_DATA_OUTPUT 3

// --------------------- IR Receiver
#define IRRECEIVER_PIN_DATA_INPUT 5

// --------------------- 433Mhz Transmitter
#define RADIO433_PIN_DATA_OUTPUT 6

// --------------------- Ultra sonic distance sensor
// --------------------- Gas Sensor

bool metric = false;

#include "common.h"
#include <MySensors.h>
#include <TaskScheduler.h>
#include "motionSensor.h"
#include "RadioBridge.h"
//#include "temphumSensor.h"
#include "bme280Sensor.h"
//#include "lightSensor.h"
#include "irBlaster.h"


// ---------------------- Scheduler
Scheduler scheduler;
void read5Min();
void read300ms();

Task slowTask(300000, TASK_FOREVER, read5Min);
Task fastTask(300, TASK_FOREVER, read300ms);

void setup()
{
  scheduler.init();
  #ifdef MY_DEBUG
    Serial.begin(115200);
    Serial.print("id: ");
    Serial.print(getNodeId());
    Serial.println();
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
    metric = getControllerConfig().isMetric;

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
}


void receive(const MyMessage &message){
  #ifdef RADIOBRIDGE
    radioBridge_receive(message);
  #endif

  #ifdef SENSOR_IRBLASTER
    irBlaster_receive(message);
  #endif
}
