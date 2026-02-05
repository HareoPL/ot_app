#ifndef MOCK_FREERTOS_SEMAPHORE_H_
#define MOCK_FREERTOS_SEMAPHORE_H_

typedef void* SemaphoreHandle_t;
#define xSemaphoreTake(sem, timeout)    (pdTRUE)
#define xSemaphoreGive(sem)             ((void)0)
#define xSemaphoreCreateMutex() ((SemaphoreHandle_t)1)

#define HRO_SEC_NOINIT /* nothing */

#endif  /* MOCK_FREERTOS_SEMAPHORE_H_ */
