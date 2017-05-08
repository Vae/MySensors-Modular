#include <TaskScheduler.h>

#define SENSOR_MOTION

#define CHILD_ID_MOTION 3

bool motion_tripped = false;
bool motion_tripped_state = false;
void motion_stop();
Task motionTask(MOTION_TIMEOUT, TASK_ONCE, motion_stop);

MyMessage msgMotion(CHILD_ID_MOTION, V_TRIPPED);

void interrupt_motion();

void motion_setup(Scheduler &scheduler){
  #ifdef MY_DEBUG
    Serial.println("### SETUP # MOTION SENSOR #####");
  #endif
  pinMode(MOTION_PIN_DATA_INPUT, INPUT);      // sets the motion sensor digital pin as input
  attachInterrupt(digitalPinToInterrupt(MOTION_PIN_DATA_INPUT), interrupt_motion, RISING);
  
  scheduler.addTask(motionTask);
}

void motion_presentation(){
  #ifdef MY_DEBUG
    Serial.println("##### PRESENTATION # MOTION SENSOR #####");
  #endif
  present(CHILD_ID_MOTION, S_MOTION, LOCATION_DESCRIPTION);
}

void motion_report(){
  #ifdef MY_DEBUG
    Serial.print("Motion: Report: ");
    Serial.print(motion_tripped_state?"1":"0");
    Serial.println();
  #endif
  send(msgMotion.set(motion_tripped_state?"1":"0"));
}

void motion_check(){
  if(motion_tripped){
    motion_tripped = false;
    motion_tripped_state = true;
    motion_report();
    motionTask.restartDelayed(MOTION_TIMEOUT);
    #ifdef MY_DEBUG
      Serial.println("Motion: Detected");
    #endif
  }
}

void interrupt_motion(){
  motion_tripped = true;
}

void motion_stop(){
  #ifdef MY_DEBUG
    Serial.println("Motion: Stopped");
  #endif
  motion_tripped_state = false;
  motion_report();
}

