#include "mock_freertos_semaphore_pthread.h"
#include <pthread.h>

static pthread_mutex_t real_host_mutex; 
static int is_mutex_initialized = 0;
static uint8_t mock_rtos_pthread_mutex_enable = 0;

SemaphoreHandle_t xSemaphoreCreateMutex(void) 
{
    if (!is_mutex_initialized) 
    {

        pthread_mutex_init(&real_host_mutex, NULL);
        is_mutex_initialized = 1;
    }
    return (SemaphoreHandle_t)1; 
}

int xSemaphoreTake(SemaphoreHandle_t sem, TickType_t timeout) 
{
    (void)sem; (void)timeout;

    if(mock_rtos_pthread_mutex_enable)
    {
        pthread_mutex_lock(&real_host_mutex); 
    }
    return pdTRUE;
}

void xSemaphoreGive(SemaphoreHandle_t sem) 
{
    (void)sem;
    
    pthread_mutex_unlock(&real_host_mutex);
}

void mock_rtos_pthread_mutex_onOff(uint8_t onOff)
{
    mock_rtos_pthread_mutex_enable = onOff;
}