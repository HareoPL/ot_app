#include "unity_fixture.h"
#include "string.h"
#include "ot_app_msg_tlv.h"

#define TEST_MSG_TLV_BUF_SIZE 256
#define TEST_MSG_TLV_KEY_1  0xAAA1
#define TEST_MSG_TLV_KEY_2  0xEEBB
#define TEST_MSG_TLV_ONE_KEY_LENGTH_BYTES 4
#define TEST_MSG_TLV_RESERVED_BYTES 2
#define TEST_MSG_TLV_RESERVED_BYTES_FOR_1_KEY_INFO (TEST_MSG_TLV_ONE_KEY_LENGTH_BYTES + TEST_MSG_TLV_RESERVED_BYTES)



static uint8_t buffer[TEST_MSG_TLV_BUF_SIZE];
static uint8_t value[10] = {1,2,3,4,5,6,7,8,9,10};

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

TEST(ot_app_msg_tlv, GivenBufSizeForOneKey_WhenCallKeyAddTwice_ThenReturnErrorNoSpace)
{
    int8_t result;

    otapp_msg_tlv_keyAdd(buffer, TEST_MSG_TLV_RESERVED_BYTES_FOR_1_KEY_INFO + 10, TEST_MSG_TLV_KEY_1, 10, value); 
    result = otapp_msg_tlv_keyAdd(buffer, TEST_MSG_TLV_RESERVED_BYTES_FOR_1_KEY_INFO + 10, TEST_MSG_TLV_KEY_1, 10, value); 
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_ERROR_NO_SPACE, result);
}

TEST(ot_app_msg_tlv, GivenBufSizeForTwoDiffrentKeys_WhenCallKeyAddTwice_ThenReturnOk)
{
    int8_t result;
    uint16_t freeBufSpace = 0;
    const uint16_t valueLength = 10;
    const uint16_t bufferSize = (2 * (TEST_MSG_TLV_ONE_KEY_LENGTH_BYTES + valueLength)) + TEST_MSG_TLV_RESERVED_BYTES; 

    otapp_msg_tlv_keyAdd(buffer, bufferSize, TEST_MSG_TLV_KEY_1, valueLength, value); 
    result = otapp_msg_tlv_keyAdd(buffer, bufferSize, TEST_MSG_TLV_KEY_2, valueLength, value); 
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_OK, result);

    result = otapp_msg_tlv_freeBufSpaceGet(buffer, bufferSize, &freeBufSpace);
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_OK, result);
    TEST_ASSERT_EQUAL(0, freeBufSpace);
}

TEST(ot_app_msg_tlv, GivenBufSizeForTwoSameKeys_WhenCallKeyAddTwice_ThenReturnKeyExist)
{
    int8_t result;
    const uint16_t valueLength = 10;
    const uint16_t bufferSize = (2 * (TEST_MSG_TLV_ONE_KEY_LENGTH_BYTES + valueLength)) + TEST_MSG_TLV_RESERVED_BYTES; 

    otapp_msg_tlv_keyAdd(buffer, bufferSize, TEST_MSG_TLV_KEY_1, valueLength, value); 
    result = otapp_msg_tlv_keyAdd(buffer, bufferSize, TEST_MSG_TLV_KEY_1, valueLength, value); 
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_KEY_EXIST, result);
}

TEST(ot_app_msg_tlv, GivenBufSizeFor10DiffrentKeys_WhenCallKeyAddTwice_ThenReturnOk)
{
    int8_t result;
    uint16_t freeBufSpace = 0;
    const uint16_t valueLength = 10;
    const uint16_t bufferSize = (10 * (TEST_MSG_TLV_ONE_KEY_LENGTH_BYTES + valueLength)) + TEST_MSG_TLV_RESERVED_BYTES; 

    for (uint8_t i = 0; i < 10; i++)
    {
        result = otapp_msg_tlv_keyAdd(buffer, bufferSize, TEST_MSG_TLV_KEY_1 + i, valueLength, value); 
        TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_OK, result);
    }    

    result = otapp_msg_tlv_freeBufSpaceGet(buffer, bufferSize, &freeBufSpace);
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_OK, result);
    TEST_ASSERT_EQUAL(0, freeBufSpace);
}

// int8_t otapp_msg_tlv_keyGet(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key, uint16_t *valueLengthOut, uint8_t *valueOut)
TEST(ot_app_msg_tlv, GivenNullPtr_WhenCallKeyGet_ThenReturnError)
{    
    int8_t result;

    result = otapp_msg_tlv_keyGet(NULL, TEST_MSG_TLV_BUF_SIZE, TEST_MSG_TLV_KEY_1, NULL, NULL);
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_ERROR, result);
}

TEST(ot_app_msg_tlv, GivenTooSmallBufSize_WhenCallKeyGet_ThenReturnError)
{    
    int8_t result;

    result = otapp_msg_tlv_keyGet(buffer, 4, TEST_MSG_TLV_KEY_1, NULL, NULL);
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_ERROR, result);
}

TEST(ot_app_msg_tlv, GivenTrueKey_WhenCallKeyGet_ThenReturnKeyExist)
{       
    int8_t result;
    uint16_t valueLength_ = 0;
    uint8_t value_[10];

    const uint16_t valueLength = 10;
    const uint16_t bufferSize = (2 * (TEST_MSG_TLV_ONE_KEY_LENGTH_BYTES + valueLength)) + TEST_MSG_TLV_RESERVED_BYTES; 

    otapp_msg_tlv_keyAdd(buffer, bufferSize, TEST_MSG_TLV_KEY_1, valueLength, value); 
    otapp_msg_tlv_keyAdd(buffer, bufferSize, TEST_MSG_TLV_KEY_2, valueLength, value); 

    result = otapp_msg_tlv_keyGet(buffer, bufferSize, TEST_MSG_TLV_KEY_1, &valueLength_, value_);
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_KEY_EXIST, result);
    TEST_ASSERT_EQUAL(valueLength, valueLength_);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(value, value_, valueLength_);
}

TEST(ot_app_msg_tlv, GivenTrueKeyAndReadNotExistKey_WhenCallKeyGet_ThenReturnKeyNoExist)
{       
    int8_t result;

    const uint16_t valueLength = 10;
    const uint16_t bufferSize = (2 * (TEST_MSG_TLV_ONE_KEY_LENGTH_BYTES + valueLength)) + TEST_MSG_TLV_RESERVED_BYTES; 

    otapp_msg_tlv_keyAdd(buffer, bufferSize, TEST_MSG_TLV_KEY_1, valueLength, value); 

    result = otapp_msg_tlv_keyGet(buffer, bufferSize, TEST_MSG_TLV_KEY_2, NULL, NULL);
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_KEY_NO_EXIST, result);
}

TEST(ot_app_msg_tlv, GivenTrueKeyAndReadExistKeyWithoutReadValueAndValSize_WhenCallKeyGet_ThenReturnKeyExist)
{       
    int8_t result;

    const uint16_t valueLength = 10;
    const uint16_t bufferSize = (2 * (TEST_MSG_TLV_ONE_KEY_LENGTH_BYTES + valueLength)) + TEST_MSG_TLV_RESERVED_BYTES; 

    otapp_msg_tlv_keyAdd(buffer, bufferSize, TEST_MSG_TLV_KEY_1, valueLength, value); 

    result = otapp_msg_tlv_keyGet(buffer, bufferSize, TEST_MSG_TLV_KEY_1, NULL, NULL);
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_KEY_EXIST, result);
}

TEST(ot_app_msg_tlv, GivenEmptyBuffer_WhenCallKeyGet_ThenReturn)
{       
    int8_t result;

    result = otapp_msg_tlv_keyGet(buffer, TEST_MSG_TLV_BUF_SIZE, TEST_MSG_TLV_KEY_1, NULL, NULL);
    TEST_ASSERT_EQUAL(OT_APP_MSG_TLV_EMPTY_BUFFER, result);
}