#include "unity_fixture.h"
#include "ot_app_pair.h"

#define UT_OAP_DEVICE_INDEX_0 (0)
#define UT_OAP_DEVICE_INDEX_1 (1)
#define UT_OAP_DEVICE_INDEX_BAD (OTAPP_PAIR_DEVICES_MAX)

#define UT_OAP_URI_INDEX_BAD    (OTAPP_PAIR_URI_MAX)
#define UT_OAP_URI_INDEX_OK     (OTAPP_URI_WELL_KNOWN_CORE)
#define UT_OAP_URI_INDEX_0      (0)
#define UT_OAP_URI_INDEX_1      (1)
#define UT_OAP_URI_INDEX_MAX    (OTAPP_PAIR_URI_MAX - 1)

#define UT_OAP_URI_TABLE_1      (OTAPP_URI_WELL_KNOWN_CORE)
#define UT_OAP_URI_TABLE_2      (OTAPP_URI_TEST)
#define UT_OAP_URI_TABLE_20     (OTAPP_URI_MOCK_20)

#define EXPECTED_URI_RETURN(devIndex, uriIndex) ((devIndex << 8) | (uriIndex)) 

static char *deviceNameFull_0 = {"device1_1_588c81fffe301ea1"};
static char *deviceNameFull_1 = {"device1_1_588c81fffe301ea2"};
static char *deviceNameFull_2 = {"device1_1_588c81fffe301ea3"};
static char *deviceNameFull_3 = {"device1_1_588c81fffe301ea4"};
static char *deviceNameFull_4 = {"device1_1_588c81fffe301ea5"};
static char *deviceNameFull_5 = {"device1_1_588c81fffe301ea6"};
static char *deviceNameFull_6 = {"device1_1_588c81fffe301ea7"};
static char *deviceNameFull_7 = {"device1_1_588c81fffe301ea8"};
static char *deviceNameFull_8 = {"device1_1_588c81fffe301ea9"};
static char *deviceNameFull_9 = {"device1_1_588c81fffe301e10"};

static otIp6Address ipAddr = {
    .mFields.m8 = {0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
                   0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x34}
};

static void ut_oap_DeviceUriIndexAddFillAll(otapp_coap_uriTableIndex_t uriIndex)
{
    for (uint8_t i = 0; i < OTAPP_PAIR_URI_MAX; i++)
    {
       otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_0, uriIndex);
       otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_1, uriIndex);
       otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_2, uriIndex);
       otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_3, uriIndex);
       otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_4, uriIndex);
       otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_5, uriIndex);
       otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_6, uriIndex);
       otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_7, uriIndex);
       otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_8, uriIndex);
       otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_9, uriIndex);
    }
}

static int8_t ut_oap_deviceAddFullFill(void)
{
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_0, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_1, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_2, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_3, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_4, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_5, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_6, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_7, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_8, &ipAddr);
    return otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_9, &ipAddr);
}

TEST_GROUP(ot_app_pair_UriIndex);

TEST_SETUP(ot_app_pair_UriIndex)
{
    otapp_pair_DeviceDeleteAll(otapp_pair_getHandle());
    
    ut_oap_deviceAddFullFill();
}

TEST_TEAR_DOWN(ot_app_pair_UriIndex)
{
    /* Cleanup after every test */
}

TEST(ot_app_pair_UriIndex, GivenNullDeviceList_WhenCallingUriIndexAdd_ThenReturnError)
{
    int16_t result;    
    result = otapp_pair_DeviceUriIndexAdd(NULL, deviceNameFull_0, UT_OAP_URI_INDEX_OK);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair_UriIndex, GivenNullDeviceName_WhenCallingUriIndexAdd_ThenReturnError)
{
    int16_t result;    
    result = otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), NULL, UT_OAP_URI_INDEX_OK);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair_UriIndex, GivenBadUri_WhenCallingUriIndexAdd_ThenReturnError)
{
    int16_t result;    
    result = otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_0, OTAPP_PAIR_NO_URI);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair_UriIndex, GivenTrueUri_WhenCallingUriIndexAdd_ThenReturn0)
{
    int16_t result;    
    result = otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_0, UT_OAP_URI_INDEX_OK);

    TEST_ASSERT_EQUAL(EXPECTED_URI_RETURN(0, 0), result);   
}

TEST(ot_app_pair_UriIndex, GivenMaxUri_WhenCallingUriIndexAdd_ThenReturn19)
{
    int16_t result;    
    for (uint8_t i = 0; i < OTAPP_PAIR_URI_MAX; i++)
    {
       result = otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_0, i+1);
    }

    TEST_ASSERT_EQUAL(EXPECTED_URI_RETURN(0, 19), result);   
}

TEST(ot_app_pair_UriIndex, GivenMaxUriPlus1_WhenCallingUriIndexAdd_ThenReturnError)
{
    int16_t result;    
    for (uint8_t i = 0; i < OTAPP_PAIR_URI_MAX + 1; i++)
    {
       result = otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_0, i+1);
    } 

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

//////////////////////
// DeviceUriIndexGet()
TEST(ot_app_pair_UriIndex, GivenNullDeviceList_WhenCallingDeviceUriIndexGet_ThenReturnError)
{
    int16_t result;    
    result = otapp_pair_deviceUriIndexGet(NULL, UT_OAP_DEVICE_INDEX_0, UT_OAP_URI_INDEX_OK);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}


TEST(ot_app_pair_UriIndex, GivenBadIndexDevice_WhenCallingDeviceUriIndexGet_ThenReturnError)
{
    int16_t result;    
    result = otapp_pair_deviceUriIndexGet(otapp_pair_getHandle(), UT_OAP_DEVICE_INDEX_BAD, UT_OAP_URI_INDEX_OK);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair_UriIndex, GivenBadIndexUri_WhenCallingDeviceUriIndexGet_ThenReturnError)
{
    int16_t result;    
    result = otapp_pair_deviceUriIndexGet(otapp_pair_getHandle(), UT_OAP_DEVICE_INDEX_0, UT_OAP_URI_INDEX_BAD);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair_UriIndex, GivenCorrectArguments_WhenCallingDeviceUriIndexGet_ThenReturn1)
{
    int16_t result;  

    ut_oap_DeviceUriIndexAddFillAll(UT_OAP_URI_TABLE_1);

    result = otapp_pair_deviceUriIndexGet(otapp_pair_getHandle(), UT_OAP_DEVICE_INDEX_0, UT_OAP_URI_INDEX_0);

    TEST_ASSERT_EQUAL(UT_OAP_URI_TABLE_1, result);
}

TEST(ot_app_pair_UriIndex, GivenIndexUriMax_WhenCallingDeviceUriIndexGet_ThenReturn2)
{
    int16_t result;   

    ut_oap_DeviceUriIndexAddFillAll(UT_OAP_URI_TABLE_2);

    result = otapp_pair_deviceUriIndexGet(otapp_pair_getHandle(), UT_OAP_DEVICE_INDEX_1, UT_OAP_URI_INDEX_MAX);

    TEST_ASSERT_EQUAL(UT_OAP_URI_TABLE_2, result);
}

TEST(ot_app_pair_UriIndex, GivenNotExistDeviceName_WhenCallingDeviceUriIndexGet_ThenReturnError)
{
    int16_t result;   
    int8_t deletedDeviceIndex; 
    ut_oap_DeviceUriIndexAddFillAll(UT_OAP_URI_TABLE_2);

    deletedDeviceIndex = otapp_pair_DeviceDelete(otapp_pair_getHandle(),deviceNameFull_6);
    result = otapp_pair_deviceUriIndexGet(otapp_pair_getHandle(), deletedDeviceIndex, UT_OAP_URI_INDEX_MAX);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_NO_EXIST, result);
} 

TEST(ot_app_pair_UriIndex, GivenNotExistUriMax_WhenCallingDeviceUriIndexGet_ThenReturnError)
{
    int16_t result;   
    int8_t deviceIndex; 
    ut_oap_DeviceUriIndexAddFillAll(UT_OAP_URI_TABLE_2);

    otapp_pair_DeviceDelete(otapp_pair_getHandle(),deviceNameFull_6);
    deviceIndex = otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_6, &ipAddr);
    result = otapp_pair_deviceUriIndexGet(otapp_pair_getHandle(), deviceIndex, UT_OAP_URI_INDEX_MAX);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_NO_URI, result);
} 

TEST(ot_app_pair_UriIndex, GivenNewUri_WhenAfterDeletedDeviceCallingDeviceUriIndexGet_ThenReturn1)
{
    int16_t result;   
    int16_t deviceIndex; 
    ut_oap_DeviceUriIndexAddFillAll(UT_OAP_URI_TABLE_2);

    otapp_pair_DeviceDelete(otapp_pair_getHandle(),deviceNameFull_6);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_6, &ipAddr);
    deviceIndex = otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_6, UT_OAP_URI_TABLE_1);
    
    result = otapp_pair_deviceUriIndexGet(otapp_pair_getHandle(), (deviceIndex >> 8), (deviceIndex & 0xFF));

    TEST_ASSERT_EQUAL(UT_OAP_URI_TABLE_1, result);
} 