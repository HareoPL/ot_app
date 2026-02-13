#include "unity_fixture.h"
#include <pthread.h>
#include <unistd.h> // for usleep()
#include <sched.h>  // for sched_yield()

#include "ot_app_buffer.h"
#include "mock_freertos_semaphore_pthread.h"

#define WRITES_PER_THREAD 10000
#define TEST_OT_APP_BUF_MUTEX_ON 1
#define TEST_OT_APP_BUF_MUTEX_OFF 0

#define TEST_OTAPP_BUF_KEY_NOT_EXIST    0x3001
static uint16_t test_otapp_buf_key = OTAPP_BUF_KEY_1;

#define TEST_OT_APP_BUF_SIZE 32
static const uint8_t data[32] = {
    0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xCC, 0xDD, // 8
    0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xCC, 0xDD,
    0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xCC, 0xDD, 
    0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xCC, 0xDD
    };


TEST_GROUP(ot_app_buffer);

TEST_SETUP(ot_app_buffer)
{
    /* Init before every test */
    mock_rtos_pthread_mutex_onOff(TEST_OT_APP_BUF_MUTEX_ON);
    otapp_buffer_init();
    otapp_buff_clear(test_otapp_buf_key);
    otapp_buf_writeUnlock(test_otapp_buf_key);
}

TEST_TEAR_DOWN(ot_app_buffer)
{
    /* Cleanup after every test */
}

static int8_t test_ot_app_buff_appendByte(uint16_t cnt, uint8_t byte)
{
    int8_t result;
    for (uint16_t i = 0; i < cnt; i++)
    {
        result = otapp_buf_append(test_otapp_buf_key, &byte, 1);
    }
    return result;
}
static void* buffer_bomber_thread(void* arg) 
{
    (void)arg;
    uint8_t data_chunk = 0xAA; // one byte of data
    
    // Each thread tries to write WRITES_PER_THREAD bytes (1 byte each). 
    // If max_size is e.g. 10,000, then most of these entries must 
    // return OTAPP_BUF_ERROR_OVERFLOW and don't corrupt the buffer!
    for(int i = 0; i < WRITES_PER_THREAD; i++) 
    {        
        otapp_buf_append(test_otapp_buf_key, &data_chunk, 1); 

        sched_yield(); 
        // usleep(1);
    }    
    return NULL;
}

uint16_t test_ot_app_buff_start_multithreaded_race_condition(uint8_t mutexState)
{
    mock_rtos_pthread_mutex_onOff(mutexState);    
    
    // Handles for our 3 threads
    pthread_t thread1, thread2, thread3;

    // launch all threads in the same fraction of a second
    pthread_create(&thread1, NULL, buffer_bomber_thread, NULL);
    pthread_create(&thread2, NULL, buffer_bomber_thread, NULL);
    pthread_create(&thread3, NULL, buffer_bomber_thread, NULL);

    // The main test WAITS until all threads have finished the for() loop
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    // read length of written bytes to buffer
    return otapp_buf_getCurrentLenSize(test_otapp_buf_key);    
}

// otapp_buf_append
TEST(ot_app_buffer, given_false_args_when_call_append_return_error)
{
    int8_t result = 0;

    result = otapp_buf_append(0, data, 4);
    TEST_ASSERT_EQUAL(OTAPP_BUF_ERROR, result);

    result = otapp_buf_append(test_otapp_buf_key, NULL, 4);
    TEST_ASSERT_EQUAL(OTAPP_BUF_ERROR, result);

    result = otapp_buf_append(test_otapp_buf_key, data, 0);
    TEST_ASSERT_EQUAL(OTAPP_BUF_ERROR, result);
}

TEST(ot_app_buffer, given_key_no_exist_when_call_append_return_error)
{
    int8_t result = 0;   

    result = otapp_buf_append(TEST_OTAPP_BUF_KEY_NOT_EXIST, data, 4);
    TEST_ASSERT_EQUAL(OTAPP_BUF_ERROR_KEY_NOT_FOUND, result);
}

TEST(ot_app_buffer, given_too_large_buf_size_when_call_append_return_overflow)
{
    int8_t result = 0;   

    result = otapp_buf_append(test_otapp_buf_key, data, OTAPP_BUF_KEY_1_SIZE + 1);
    TEST_ASSERT_EQUAL(OTAPP_BUF_ERROR_OVERFLOW, result);
}

TEST(ot_app_buffer, given_tru_args_when_call_append_return_ok)
{
    const uint8_t dataLen = 4;
    int8_t result = 0;
    uint16_t currentLen = 0;
    uint8_t dataRead[dataLen];
    uint16_t readLen = 0;

    result = otapp_buf_append(test_otapp_buf_key, data, dataLen);
    TEST_ASSERT_EQUAL(OTAPP_BUF_OK, result);

    currentLen = otapp_buf_getCurrentLenSize(test_otapp_buf_key);
    TEST_ASSERT_EQUAL(dataLen, currentLen);

    result = otapp_buf_getData(test_otapp_buf_key, dataRead, dataLen, &readLen);
    TEST_ASSERT_EQUAL(OTAPP_BUF_OK, result);
    TEST_ASSERT_EQUAL_INT8_ARRAY(data, dataRead, dataLen);
}

TEST(ot_app_buffer, given_tru_args_when_call_append_twice_return_ok)
{
    const uint8_t dataSize = 8;
    int8_t result = 0;
    uint16_t currentLen = 0;
    uint8_t dataRead[dataSize];
    uint16_t readLen = 0;

    for (uint8_t i = 0; i < dataSize; i++)
    {
        result = otapp_buf_append(test_otapp_buf_key, &data[i], 1);
        TEST_ASSERT_EQUAL(OTAPP_BUF_OK, result);
    }
    
    currentLen = otapp_buf_getCurrentLenSize(test_otapp_buf_key);
    TEST_ASSERT_EQUAL(dataSize, currentLen);

    result = otapp_buf_getData(test_otapp_buf_key, dataRead, dataSize, &readLen);
    TEST_ASSERT_EQUAL(OTAPP_BUF_OK, result);
    TEST_ASSERT_EQUAL_INT8_ARRAY(data, dataRead, dataSize);
}

// otapp_buf_getData
TEST(ot_app_buffer, given_false_args_when_call_getData_return_error)
{
    int8_t result = 0;
    uint16_t lenBufOut = 0;
    uint8_t dataRead[4];

    result = otapp_buf_getData(0, dataRead, 4, &lenBufOut);
    TEST_ASSERT_EQUAL(OTAPP_BUF_ERROR, result);

    result = otapp_buf_getData(test_otapp_buf_key, NULL, 4, &lenBufOut);
    TEST_ASSERT_EQUAL(OTAPP_BUF_ERROR, result);

    result = otapp_buf_getData(test_otapp_buf_key, dataRead, 0, &lenBufOut);
    TEST_ASSERT_EQUAL(OTAPP_BUF_ERROR, result);

    result = otapp_buf_getData(test_otapp_buf_key, dataRead, 4, NULL);
    TEST_ASSERT_EQUAL(OTAPP_BUF_ERROR, result);
}

TEST(ot_app_buffer, given_too_small_buf_when_call_getData_return_error)
{
    const uint8_t appendCnt = 4;
    int8_t result = 0;
    uint8_t dataRead[appendCnt];
    uint16_t lenBufOut = 0;
   
    result = test_ot_app_buff_appendByte(appendCnt, 0xAA);
    TEST_ASSERT_EQUAL(OTAPP_BUF_OK, result);

    result = otapp_buf_getData(test_otapp_buf_key, dataRead, appendCnt - 1, &lenBufOut);
    TEST_ASSERT_EQUAL(OTAPP_BUF_ERROR, result);
}

TEST(ot_app_buffer, given_correct_args_when_call_getData_return_ok)
{
    const uint8_t appendCnt = 4;
    const uint8_t value = 0xAA;
    int8_t result = 0;
    uint8_t dataRead[appendCnt];
    uint16_t lenBufOut = 0;
   
    result = test_ot_app_buff_appendByte(appendCnt, value);
    TEST_ASSERT_EQUAL(OTAPP_BUF_OK, result);

    result = otapp_buf_getData(test_otapp_buf_key, dataRead, appendCnt, &lenBufOut);
    TEST_ASSERT_EQUAL(OTAPP_BUF_OK, result);
    TEST_ASSERT_EQUAL(appendCnt, lenBufOut);

    for (uint8_t i = 0; i < appendCnt; i++)
    {
        TEST_ASSERT_EQUAL(value, dataRead[i]);
    }
}

// otapp_buf_getReadOnly_ptr
TEST(ot_app_buffer, given_false_args_when_call_getReadOnly_ptr_return_error)
{
    const uint8_t *dataPtr = NULL;
    uint16_t bufSizeOut = 0;

    dataPtr = otapp_buf_getReadOnly_ptr(0, &bufSizeOut);
    TEST_ASSERT_EQUAL(NULL, dataPtr);

    dataPtr = otapp_buf_getReadOnly_ptr(test_otapp_buf_key, NULL);
    TEST_ASSERT_EQUAL(NULL, dataPtr);
}

TEST(ot_app_buffer, given_true_args_when_call_getReadOnly_ptr_return_not_null_ptr)
{
    const uint8_t *dataPtr = NULL;
    uint16_t bufSizeOut = 0;

    dataPtr = otapp_buf_getReadOnly_ptr(test_otapp_buf_key, &bufSizeOut);
    TEST_ASSERT_NOT_EQUAL(NULL, dataPtr);
}

TEST(ot_app_buffer, given_append_when_call_getReadOnly_ptr_return_ok)
{
    const uint8_t appendCnt = 4;
    const uint8_t value = 0xAA;
    const uint8_t *dataPtr = NULL;
    int8_t result = 0;
    uint16_t bufSizeOut = 0;

    result = test_ot_app_buff_appendByte(appendCnt, value);
    TEST_ASSERT_EQUAL(OTAPP_BUF_OK, result);

    dataPtr = otapp_buf_getReadOnly_ptr(test_otapp_buf_key, &bufSizeOut);
    TEST_ASSERT_NOT_EQUAL(NULL, dataPtr);
    TEST_ASSERT_EQUAL(appendCnt, appendCnt);

    for (uint8_t i = 0; i < appendCnt; i++)
    {
       TEST_ASSERT_EQUAL(value, *(dataPtr+i));
    }   
}

//otapp_buf_getWriteOnly_ptr
TEST(ot_app_buffer, given_false_args_when_call_getWriteOnly_ptr_return_error)
{
    const uint16_t bufSize = 16;
    uint8_t *dataPtr = NULL;

    dataPtr = otapp_buf_getWriteOnly_ptr(0, bufSize);
    TEST_ASSERT_EQUAL(NULL, dataPtr);

    dataPtr = otapp_buf_getWriteOnly_ptr(test_otapp_buf_key, 0);
    TEST_ASSERT_EQUAL(NULL, dataPtr);
}

TEST(ot_app_buffer, given_too_large_size_when_call_getWriteOnly_ptr_return_error)
{
    uint8_t *dataPtr = NULL;

    dataPtr = otapp_buf_getWriteOnly_ptr(test_otapp_buf_key, OTAPP_BUF_KEY_1_SIZE + 1);
    TEST_ASSERT_EQUAL(NULL, dataPtr);
}

TEST(ot_app_buffer, check_lock_when_call_getWriteOnly_ptr_and_append_return_error)
{
    const uint8_t value = 0xAA;
    const uint16_t bufSize = 16;
    uint8_t *dataPtr = NULL;
    int8_t result;

    dataPtr = otapp_buf_getWriteOnly_ptr(test_otapp_buf_key, bufSize);
    TEST_ASSERT_NOT_EQUAL(NULL, dataPtr);

    dataPtr = NULL;
    dataPtr = otapp_buf_getWriteOnly_ptr(test_otapp_buf_key, bufSize);
    TEST_ASSERT_EQUAL(NULL, dataPtr);

    // append po getWriteOnly_ptr(). Powinien byc zablokowany. 
    result = otapp_buf_append(test_otapp_buf_key, &value, 1);
    TEST_ASSERT_EQUAL(OTAPP_BUF_ERROR_WRITE_LOCK, result);
}

TEST(ot_app_buffer, check_unlock_when_call_getWriteOnly_ptr_and_append_return_ok)
{
    const uint8_t value = 0xAA;
    const uint16_t bufSize = 16;
    uint8_t *dataPtr = NULL;
    int8_t result;

    dataPtr = otapp_buf_getWriteOnly_ptr(test_otapp_buf_key, bufSize);
    TEST_ASSERT_NOT_EQUAL(NULL, dataPtr);

    result = otapp_buf_writeUnlock(test_otapp_buf_key);
    TEST_ASSERT_EQUAL(OTAPP_BUF_OK, result);

    dataPtr = NULL;
    dataPtr = otapp_buf_getWriteOnly_ptr(test_otapp_buf_key, bufSize);
    TEST_ASSERT_NOT_EQUAL(NULL, dataPtr);

    result = otapp_buf_writeUnlock(test_otapp_buf_key);
    TEST_ASSERT_EQUAL(OTAPP_BUF_OK, result);
    
    result = otapp_buf_append(test_otapp_buf_key, &value, 1);
    TEST_ASSERT_EQUAL(OTAPP_BUF_OK, result);
}

TEST(ot_app_buffer, given_true_args_when_call_getWriteOnly_ptr_return_ok)
{
    const uint8_t value = 0xAA;
    const uint16_t bufSize = 16;
    uint8_t *dataPtr = NULL;
    int8_t result;

    // rezerwujemy buffor dla dataPtr
    dataPtr = otapp_buf_getWriteOnly_ptr(test_otapp_buf_key, bufSize);
    TEST_ASSERT_NOT_EQUAL(NULL, dataPtr);
    
    // wgrywanie danych do buff przez ptr
    for (uint16_t i = 0; i < bufSize; i++)
    {
        *(dataPtr+i) = value;
    }
    
    // sprawdzanie poprawnosci zapisu danych do buf ptr
    for (uint16_t i = 0; i < bufSize; i++)
    {
       TEST_ASSERT_EQUAL(value, *(dataPtr+i));
    } 

    result = otapp_buf_writeUnlock(test_otapp_buf_key);
    TEST_ASSERT_EQUAL(OTAPP_BUF_OK, result);   
}

//////////////////////////////
// multithread race condidion
TEST(ot_app_buffer, buffer_should_survive_multithreaded_race_condition_when_mutex_on_retorn_ok)
{
    uint16_t finalLen = test_ot_app_buff_start_multithreaded_race_condition(TEST_OT_APP_BUF_MUTEX_ON);
    uint16_t maxSize = otapp_buf_getMaxSize(test_otapp_buf_key);
    TEST_ASSERT_NOT_EQUAL(0, maxSize);

    TEST_ASSERT_EQUAL_MESSAGE(maxSize, finalLen, 
        "Race error! Either the Bounds Check was interrupted or the len variable increments were lost.");
}

TEST(ot_app_buffer, buffer_should_not_survive_multithreaded_race_condition_when_mutex_off_return_error)
{
    uint16_t finalLen = test_ot_app_buff_start_multithreaded_race_condition(TEST_OT_APP_BUF_MUTEX_OFF);
    uint16_t maxSize = otapp_buf_getMaxSize(test_otapp_buf_key);
    TEST_ASSERT_NOT_EQUAL(0, maxSize);

    TEST_ASSERT_NOT_EQUAL_MESSAGE(maxSize, finalLen, 
        "Race error! Either the Bounds Check was interrupted or the len variable increments were lost.");
}