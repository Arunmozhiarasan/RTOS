#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// Define pins for smoke sensor, LED, and buzzer
#define SMOKE_SENSOR_PIN A0
#define LED_PIN 13
#define BUZZER_PIN 12

// Define thresholds
#define SMOKE_THRESHOLD 25

// Define task stack size
#define STACK_SIZE 128

// Task handles
TaskHandle_t taskHandle;

// Semaphore to protect shared resources
SemaphoreHandle_t sensorSemaphore;

// Function prototypes
void smokeSensorTask(void *pvParameters);

void setup() {
  Serial.begin(9600);

  // Initialize semaphore
  sensorSemaphore = xSemaphoreCreateMutex();

  // Create smoke sensor task
  xTaskCreate(smokeSensorTask, "SmokeSensorTask", STACK_SIZE, NULL, 1, &taskHandle);
}

void loop() {
  // Nothing to do here, tasks are running
}

void smokeSensorTask(void *pvParameters) {
  (void) pvParameters;

  pinMode(SMOKE_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  while (true) {
    // Read smoke sensor value
    int smokeValue = analogRead(SMOKE_SENSOR_PIN);

    // Acquire semaphore to protect shared resources
    if (xSemaphoreTake(sensorSemaphore, (TickType_t)10) == pdTRUE) {
      // Display smoke value every 5 seconds
      Serial.print("Smoke Value: ");
      Serial.println(smokeValue);
      
      // Release semaphore
      xSemaphoreGive(sensorSemaphore);
    }

    // Check if smoke value is above threshold
    if (smokeValue > SMOKE_THRESHOLD) {
      digitalWrite(LED_PIN, HIGH); // Turn on LED
      digitalWrite(BUZZER_PIN, HIGH); // Turn on buzzer
      if (xSemaphoreTake(sensorSemaphore, (TickType_t)10) == pdTRUE) {
        Serial.println("Gas Detected"); // Print gas detection message
        xSemaphoreGive(sensorSemaphore);
      }
    } else {
      digitalWrite(LED_PIN, LOW); // Turn off LED
      digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
    }

    // Wait for 5 seconds
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}
