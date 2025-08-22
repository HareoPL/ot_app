#include "unity_fixture.h"
#include "ot_app_pair.h"

#define UT_OAP_DEVICE_INDEX_0 (0)

#define EXPECTED_URI_RETURN(devIndex, uriIndex) ((devIndex << 8) | (uriIndex)) 

static char *deviceNameFull_1 = {"device1_1_588c81fffe301ea1"};
static char *deviceNameFull_2 = {"device1_1_588c81fffe301ea2"};
static char *deviceNameFull_3 = {"device1_1_588c81fffe301ea3"};
static char *deviceNameFull_4 = {"device1_1_588c81fffe301ea4"};
static char *deviceNameFull_5 = {"device1_1_588c81fffe301ea5"};
static char *deviceNameFull_6 = {"device1_1_588c81fffe301ea6"};
static char *deviceNameFull_7 = {"device1_1_588c81fffe301ea7"};
static char *deviceNameFull_8 = {"device1_1_588c81fffe301ea8"};
static char *deviceNameFull_9 = {"device1_1_588c81fffe301ea9"};
static char *deviceNameFull_10 = {"device1_1_588c81fffe301e10"};

static otIp6Address ipAddr = {
    .mFields.m8 = {0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
                   0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x34}
};

TEST_GROUP(ot_app_pair_UriIndex);

TEST_SETUP(ot_app_pair_UriIndex)
{
    otapp_pair_DeviceDeleteAll(otapp_pair_getHandle());

    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_1, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_2, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_3, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_4, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_5, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_6, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_7, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_8, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_9, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_10, &ipAddr);
}

TEST_TEAR_DOWN(ot_app_pair_UriIndex)
{
    /* Cleanup after every test */
}
// result = otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_1, OTAPP_URI_WELL_KNOWN_CORE);
TEST(ot_app_pair_UriIndex, GivenNullDeviceList_WhenCallingUriIndexAdd_ThenReturnError)
{
    int16_t result;    
    result = otapp_pair_DeviceUriIndexAdd(NULL, deviceNameFull_1, OTAPP_URI_WELL_KNOWN_CORE);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair_UriIndex, GivenNullDeviceName_WhenCallingUriIndexAdd_ThenReturnError)
{
    int16_t result;    
    result = otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), NULL, OTAPP_URI_WELL_KNOWN_CORE);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair_UriIndex, GivenBadUri_WhenCallingUriIndexAdd_ThenReturnError)
{
    int16_t result;    
    result = otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_1, OTAPP_PAIR_NO_URI);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair_UriIndex, GivenTrueUri_WhenCallingUriIndexAdd_ThenReturn0)
{
    int16_t result;    
    result = otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_1, OTAPP_URI_WELL_KNOWN_CORE);

    TEST_ASSERT_EQUAL(EXPECTED_URI_RETURN(0, 0), result);   
}

TEST(ot_app_pair_UriIndex, GivenMaxUri_WhenCallingUriIndexAdd_ThenReturn19)
{
    int16_t result;    
    for (uint8_t i = 0; i < OTAPP_PAIR_URI_MAX; i++)
    {
       result = otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_1, i+1);
    }

    TEST_ASSERT_EQUAL(EXPECTED_URI_RETURN(0, 19), result);   
}

TEST(ot_app_pair_UriIndex, GivenMaxUriPlus1_WhenCallingUriIndexAdd_ThenReturnError)
{
    int16_t result;    
    for (uint8_t i = 0; i < OTAPP_PAIR_URI_MAX + 1; i++)
    {
       result = otapp_pair_DeviceUriIndexAdd(otapp_pair_getHandle(), deviceNameFull_1, i+1);
    } 

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair_UriIndex, GivenNullDeviceList_WhenCallingDeviceUriIndexGet_ThenReturnError)
{
    int16_t result;    
    result = otapp_pair_deviceUriIndexGet(NULL, UT_OAP_DEVICE_INDEX_0, OTAPP_URI_WELL_KNOWN_CORE); // OTAPP_PAIR_NO_URI

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

