#include <Arduino.h>
#include "led/Led.h"
#include "hcsr04/HC_SR04.h"

//Les broches
const int PIN_RED = 4;
const int PIN_GREEN = 5;
const int PIN_YELLOW = 6;

const int PIN_ECHO = 7;
const int PIN_TRIG = 8;

const int PIN_ENA = 9;
const int PIN_IN1 =10;
const int PIN_IN2 = 11;


SemaphoreHandle_t distSemaphore = nullptr;
TaskHandle_t redHandle = nullptr;
TaskHandle_t motorHandle = nullptr;
TaskHandle_t yellowHandle = nullptr;
TaskHandle_t greenHandle = nullptr;
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
      xSemaphoreTake(distSemaphore, 0);
    } else if (distance >= 20 && distance < 40){
      xSemaphoreGive(distSemaphore);
    }
      vTaskDelay(pdMS_TO_TICKS(100));
  }

}
void vTaskLed(void* ptr){

  Led *led =(Led*)ptr;

  while(true){

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    led->on();
  }
}

void vTaskMotor( void* pvParam){

  UBaseType_t coins;

  while(true){

      ledGreen.off();
      ledYellow.off();
      ledRed.off();

    coins = uxSemaphoreGetCount(distSemaphore);

    if(coins==3){
      rover.setSpeed(255);
      xTaskNotifyGive(greenHandle);
      xTaskNotifyGive(yellowHandle);
      xTaskNotifyGive(redHandle);
    } else if (coins==2){
       rover.setSpeed(128);
       xTaskNotifyGive(yellowHandle);
      xTaskNotifyGive(redHandle);
    } else if (coins==1){
       rover.setSpeed(64);
     xTaskNotifyGive(redHandle);
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
        &motorHandle
    );

     xTaskCreate(
        vTaskLed,
        "GREEN",
        1024,
        &ledGreen,
        1,
        &greenHandle
    );


     xTaskCreate(
        vTaskLed,
        "YELLOW",
        1024,
        &ledYellow,
        1,
        &yellowHandle
    );


     xTaskCreate(
        vTaskLed,
        "RED",
        1024,
        &ledRed,
        1,
        &redHandle
    );


}

void loop() {
}
