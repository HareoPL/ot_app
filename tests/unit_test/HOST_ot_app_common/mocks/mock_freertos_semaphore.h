#ifndef MOCK_FREERTOS_SEMAPHORE_H_
#define MOCK_FREERTOS_SEMAPHORE_H_

#include "stdint.h"

typedef void* SemaphoreHandle_t;
// #define xSemaphoreTake(sem, timeout)    (pdTRUE)
#define xSemaphoreGive(sem)             ((void)0)
#define xSemaphoreCreateMutex() ((SemaphoreHandle_t)1)

#ifndef pdTRUE
    #define pdTRUE 1
#endif

#ifndef pdFALSE
    #define pdFALSE 0
#endif

#ifndef portMAX_DELAY
    #define portMAX_DELAY 0xFFFFFFFF
#endif
#ifndef TickType_t
    #define TickType_t uint32_t
#endif

static inline int xSemaphoreTake(SemaphoreHandle_t sem, TickType_t timeout)
{
    (void)sem;      // Uciszamy ostrzeżenie "unused parameter"
    (void)timeout;  // Uciszamy ostrzeżenie "unused parameter"
    return pdTRUE;  // Zwracamy sukces, na wypadek gdybyś użył tego w if()
}

#define HRO_SEC_NOINIT /* nothing */

#endif  /* MOCK_FREERTOS_SEMAPHORE_H_ */
