#include "unity_fixture.h"
#include "string.h"
#include "ot_app_msg_tlv.h"

#define TEST_MSG_TLV_BUF_SIZE 256
#define TEST_MSG_TLV_KEY_1  0xFF00
#define TEST_MSG_TLV_ONE_KEY_LENGTH_BYTES 2
#define TEST_MSG_TLV_RESERVED_BYTES 2
#define TEST_MSG_TLV_RESERVED_BYTES_FOR_1_KEY_INFO (TEST_MSG_TLV_ONE_KEY_LENGTH_BYTES + TEST_MSG_TLV_RESERVED_BYTES)



static uint8_t buffer[TEST_MSG_TLV_BUF_SIZE];
static uint8_t value[10] = {1,2,3,4,5,6,7,8,9,0};

void test_msg_tlv_clearBuffer(void)
{
    memset(buffer, 0, TEST_MSG_TLV_BUF_SIZE);
}

TEST_GROUP(ot_app_msg_tlv);

TEST_SETUP(ot_app_msg_tlv)
{
    /* Init before every test */
    test_msg_tlv_clearBuffer();
}

TEST_TEAR_DOWN(ot_app_msg_tlv)
{
    /* Cleanup after every test */
}

// int8_t otapp_msg_tlv_keyAdd(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key, const uint16_t valueLengthIn, uint8_t *valueIn);
TEST(ot_app_msg_tlv, GivenNullPtr_WhenCallKeyAdd_ThenReturnError)
{    
    int8_t result;

    result = otapp_msg_tlv_keyAdd(NULL,       TEST_MSG_TLV_BUF_SIZE, TEST_MSG_TLV_KEY_1, 10, value);
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_ERROR, result);

    result = otapp_msg_tlv_keyAdd(buffer,    TEST_MSG_TLV_BUF_SIZE, TEST_MSG_TLV_KEY_1, 10, NULL);
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_ERROR, result);
}

TEST(ot_app_msg_tlv, GivenFalseBufSize_WhenCallKeyAdd_ThenReturnError)
{
    int8_t result;

    result = otapp_msg_tlv_keyAdd(buffer, TEST_MSG_TLV_BUF_SIZE, TEST_MSG_TLV_KEY_1, 0, value);
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_ERROR, result);

    result = otapp_msg_tlv_keyAdd(buffer, 4, TEST_MSG_TLV_KEY_1, 10, value);
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_ERROR, result);
}

TEST(ot_app_msg_tlv, GivenSmallerBufSizeThenValue_WhenCallKeyAdd_ThenReturnError)
{
    int8_t result;

    result = otapp_msg_tlv_keyAdd(buffer, TEST_MSG_TLV_RESERVED_BYTES_FOR_1_KEY_INFO + 10 - 1, TEST_MSG_TLV_KEY_1, 10, value); 
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_ERROR, result);
}

TEST(ot_app_msg_tlv, GivenPerfectBufSizeForValueLength_WhenCallKeyAdd_ThenReturnOK)
{
    int8_t result;

    result = otapp_msg_tlv_keyAdd(buffer, TEST_MSG_TLV_RESERVED_BYTES_FOR_1_KEY_INFO + 10, TEST_MSG_TLV_KEY_1, 10, value); 
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_OK, result);
}


// int8_t otapp_msg_tlv_keyGet(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key, uint16_t *valueLengthOut, uint8_t *valueOut);cd app/ot_app_scd app/ot_app_sb
