#include <Arduino.h>
#include "led/Led.h"
#include "hcsr04/HC_SR04.h"

//Les broches
const int PIN_RED = 40;
const int PIN_GREEN = 41;
const int PIN_YELLOW = 42;

const int PIN_ECHO = 39;
const int PIN_TRIG = 38;

const int PIN_ENA = 1;
const int PIN_IN1 =2;
const int PIN_IN2 = 3;


SemaphoreHandle_t distSemaphore = nullptr;
TaskHandle_t ledHandle = nullptr;
TaskHandle_t sensorHandle = nullptr;

//Instances

Led ledGreen(PIN_GREEN);
Led ledYellow(PIN_YELLOW);
Led ledRed(PIN_RED);

HC_SR04 sensor(PIN_TRIG, PIN_ECHO);

Motor rover(PIN_ENA, PIN_IN1, PIN_IN2);

void vTaskMeasure( void* pvParam){

 float distance;

  while(true){

    distance = sensor.getDistance();

    if(distance < 20  && distance >= 0){
      xSmaphoreTake(distSemaphore, 0)
    } else if (distance >= 20 && distance < 40){
      xSemaphoreGive(distSemaphore);
    }
      vTaskDelay(pdMS_TO_TICKS(100));
  }

}

void vTaskMotor( void* pvParam){

  UBaseType_t coins;

  while(true){

      ledGreen.off();
      ledYellow.off();
      ledRed.off();

    coins = uxGetSemaphoreCount(distSemaphore);

    if(coins==3){
      rover.setSpeed(255);
      ledGreen.on();
      ledYellow.on();
      ledRed.on();
    } else if (coins==2){
       rover.setSpeed(128);
      ledYellow.on();
      ledRed.on();
    } else if (coins==1){
       rover.setSpeed(64);
      ledRed.on();
    } else {
      rover.stop();
    }
  
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() {

  //Initialisations

  ledGreen.init();
  ledRed.init();
  ledYellow.init();
  sensor.init();  
  rover.init();

   distSemaphore = xSemaphoreCreateCounting(3, 0);

    if (distSemaphore == nullptr) {
        // Arrêt de sécurité en cas d'échec
        rover.stop();
    }

    xTaskCreate(
        vTaskMeasure,
        "Measure",
        1024,
        nullptr,
        2,
        &sensorHandle
    );

    xTaskCreate(
        vTaskMotor,
        "Rover",
        1024,
        nullptr,
        1,
        &ledHandle
    );

}

void loop() {
  // put your main code here, to run repeatedly:
}
