#ifndef MOCK_FREERTOS_SEMAPHORE_PTHREAD_H_
#define MOCK_FREERTOS_SEMAPHORE_PTHREAD_H_

#include <stdint.h>

typedef void* SemaphoreHandle_t;
typedef uint32_t TickType_t;

#define pdTRUE 1
#define pdFALSE 0
#define portMAX_DELAY 0xFFFFFFFF

SemaphoreHandle_t xSemaphoreCreateMutex(void);
int xSemaphoreTake(SemaphoreHandle_t sem, TickType_t timeout);
void xSemaphoreGive(SemaphoreHandle_t sem);

void mock_rtos_pthread_mutex_onOff(uint8_t onOff);

#endif  /* MOCK_FREERTOS_SEMAPHORE_PTHREAD_H_ */
