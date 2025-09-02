#include "unity_fixture.h"
#include "ot_app_pair.h"
#include "mock_freertos_queue.h"
#include "mock_freertos_task.h"
#include "mock_ot_app_deviceName.h"

#define UT_OAP_RETURN_INDEX_0 (0)
#define UT_OAP_RETURN_INDEX_5 (5)
#define UT_OAP_RETURN_INDEX_9 (9)

#define UT_OAP_MAX_QUEUE_ITEM   OTAPP_PAIR_QUEUE_LENGTH
#define UT_OAP_RETURN_QUEUE_ITEM_QUANTITY_9 (9)
#define UT_OAP_RETURN_QUEUE_ITEM_QUANTITY_0 (0)

#define UT_OAP_IS_NOT_MATCH (OTAPP_PAIR_IS_NOT)

static char *deviceNameFull_0 = {"device1_2_588c81fffe301ea1"};
static char *deviceNameFull_1 = {"device1_2_588c81fffe301ea2"};
static char *deviceNameFull_2 = {"device1_2_588c81fffe301ea3"};
static char *deviceNameFull_3 = {"device1_2_588c81fffe301ea4"};
static char *deviceNameFull_4 = {"device1_2_588c81fffe301ea5"};
static char *deviceNameFull_5 = {"device1_2_588c81fffe301ea6"};
static char *deviceNameFull_6 = {"device1_2_588c81fffe301ea7"};
static char *deviceNameFull_7 = {"device1_2_588c81fffe301ea8"};
static char *deviceNameFull_8 = {"device1_2_588c81fffe301ea9"};
static char *deviceNameFull_9 = {"device1_2_588c81fffe301e10"};
static char *deviceNameFull_MaxLength10 = {"device1_2_588c81fffe301ea4___31"};
static char *deviceNameFull_NOT_MATCH = {"device2_2_588c81fffe301ea1"};

static otIp6Address ipAddr_ok_1 = {
    .mFields.m8 = {0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
                   0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x34}
};

otapp_pair_queueItem_t queueItem;

otapp_pair_queueItem_t *ut_oap_completeQueueItem(char *deviceNameFull, otIp6Address *ipAddress)
{
    queueItem.type = OTAPP_PAIR_CHECK_AND_ADD_TO_DEV_LIST;

    strcpy(queueItem.deviceNameFull, deviceNameFull);
    memcpy(&queueItem.ipAddress, ipAddress, sizeof(otIp6Address));

    return &queueItem;
}

int8_t ut_oap_fillQueue(uint8_t numItem)
{
    int8_t result;

    if(numItem > 11)
    {
        return -10;
    }

    char *deviceNamesFull[] = {
        deviceNameFull_0, deviceNameFull_1,
        deviceNameFull_2, deviceNameFull_3,
        deviceNameFull_4, deviceNameFull_5,
        deviceNameFull_6, deviceNameFull_7,
        deviceNameFull_8, deviceNameFull_9,
        deviceNameFull_MaxLength10
    };

    for (int i = 0; i < numItem; i++) 
    {
        otapp_pair_queueItem_t *_queueItem = ut_oap_completeQueueItem(deviceNamesFull[i], &ipAddr_ok_1);
        result = otapp_pair_addToQueue(_queueItem);
    }

    return result;
}

void ut_oap_runTask(uint8_t numTimes)
{
    for (uint8_t i = 0; i < numTimes; i++)
    {
        ft_mock_runTaskCallback();
    }       
}


TEST_GROUP(ot_app_pair_rtos);

TEST_SETUP(ot_app_pair_rtos)
{
    fq_mock_QueueDeleteAllItems();
    otapp_pair_DeviceDeleteAll(otapp_pair_getHandle());
}

TEST_TEAR_DOWN(ot_app_pair_rtos)
{
    /* Cleanup after every test */
}

////////////////////////////
// freeRtos queue, deviceIsMatchingFromQueue

TEST(ot_app_pair_rtos, GivenInitQueue_WhenCalled_ThenReturnOK)
{
    int8_t result;   

    result = otapp_pair_initQueue();

    TEST_ASSERT_EQUAL(OTAPP_PAIR_OK , result);
}

TEST(ot_app_pair_rtos, GivenInitTask_WhenCalled_ThenReturnOK)
{
    int8_t result;   

    result = otapp_pair_initTask();

    TEST_ASSERT_EQUAL(OTAPP_PAIR_OK , result);
}

TEST(ot_app_pair_rtos, GivenNullPtr_WhenCallingAddToQueue_ThenReturnError)
{
    int8_t result;   

    otapp_pair_initQueue();
    result = otapp_pair_addToQueue(NULL);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR , result);
}

TEST(ot_app_pair_rtos, GivenTrueQueueItem_WhenCallingAddToQueue_ThenReturnOK)
{
    int8_t result;   
     otapp_pair_queueItem_t *_queueItem;

    otapp_pair_initQueue();
       
    _queueItem = ut_oap_completeQueueItem(deviceNameFull_0, &ipAddr_ok_1);
    result = otapp_pair_addToQueue(_queueItem);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_OK , result);
}

TEST(ot_app_pair_rtos, GivenMaxQueueItems_WhenCallingAddToQueue_ThenReturnOK)
{
    int8_t result;

    otapp_pair_initQueue();
    
    result = ut_oap_fillQueue(UT_OAP_MAX_QUEUE_ITEM);
    
    TEST_ASSERT_EQUAL(OTAPP_PAIR_OK , result);
}

TEST(ot_app_pair_rtos, GivenMaxPlus1QueueItems_WhenCallingAddToQueue_ThenReturnError)
{
    int8_t result;

    otapp_pair_initQueue();
    result = ut_oap_fillQueue(UT_OAP_MAX_QUEUE_ITEM + 1);
    
    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR , result);
}

TEST(ot_app_pair_rtos, GivenMaxQueueItems_WhenCallingRtosTask_ThenReturnItemQuantity9)
{
    size_t result;   
    otapp_pair_queueItem_t *_queueItem;

    otapp_pair_initQueue();
    ut_oap_fillQueue(UT_OAP_MAX_QUEUE_ITEM);
    ft_mock_runTaskCallback();
    result = fq_mock_QueueGetItemQuantity();

    TEST_ASSERT_EQUAL(UT_OAP_RETURN_QUEUE_ITEM_QUANTITY_9 , result);
}

TEST(ot_app_pair_rtos, GivenMaxQueueItems_WhenCallingRtosTaskMaxTimes_ThenReturnItemQuantity0)
{
    size_t result;   
    otapp_pair_queueItem_t *_queueItem;

    otapp_pair_initQueue();
    ut_oap_fillQueue(UT_OAP_MAX_QUEUE_ITEM);

    ut_oap_runTask(UT_OAP_MAX_QUEUE_ITEM);

    result = fq_mock_QueueGetItemQuantity();

    TEST_ASSERT_EQUAL(UT_OAP_RETURN_QUEUE_ITEM_QUANTITY_0 , result);
}

TEST(ot_app_pair_rtos, GivenMaxQueueItems_WhenCallingRtosTaskMaxTimesPlus1_ThenReturnItemQuantity0)
{
    size_t result;   
    otapp_pair_queueItem_t *_queueItem;

    otapp_pair_initQueue();
    ut_oap_fillQueue(UT_OAP_MAX_QUEUE_ITEM);

    ut_oap_runTask(UT_OAP_MAX_QUEUE_ITEM + 1);

    result = fq_mock_QueueGetItemQuantity();

    TEST_ASSERT_EQUAL(UT_OAP_RETURN_QUEUE_ITEM_QUANTITY_0 , result);
}

TEST(ot_app_pair_rtos, GivenMatchQueueItem_WhenCallingIsMatchingFromQueue_ThenReturnIndex0)
{
    int8_t result;   
    otapp_pair_queueItem_t *_queueItem;

    _queueItem = ut_oap_completeQueueItem(deviceNameFull_0, &ipAddr_ok_1);
    result = otapp_pair_deviceIsMatchingFromQueue(otapp_pair_getHandle(), _queueItem);
    
    TEST_ASSERT_EQUAL(UT_OAP_RETURN_INDEX_0 , result);
}

TEST(ot_app_pair_rtos, GivenNotMatchQueueItem_WhenCallingIsMatchingFromQueue_ThenReturnError)
{
    int8_t result;   
    otapp_pair_queueItem_t *_queueItem;

    _queueItem = ut_oap_completeQueueItem(deviceNameFull_NOT_MATCH, &ipAddr_ok_1);
    mock_oadevName_state(UT_OAP_IS_NOT_MATCH);
    result = otapp_pair_deviceIsMatchingFromQueue(otapp_pair_getHandle(), _queueItem);
    
    TEST_ASSERT_EQUAL(UT_OAP_IS_NOT_MATCH , result);
}
