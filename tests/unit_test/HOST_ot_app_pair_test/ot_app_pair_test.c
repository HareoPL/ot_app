#include "unity_fixture.h"
#include "ot_app_pair.h"

#define UT_OAP_RETURN_INDEX_0 (0)
#define UT_OAP_RETURN_INDEX_9 (9)

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
static char *deviceNameFull_MaxLength10 = {"device1_1_588c81fffe301ea4___31"};
static char *deviceNameFull_TooLength = {"device1_1_588c81fffe301ea9____32"};

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


TEST_GROUP(ot_app_pair);

TEST_SETUP(ot_app_pair)
{
   otapp_pair_DeviceDeleteAll(otapp_pair_getHandle());
}

TEST_TEAR_DOWN(ot_app_pair)
{
    /* Cleanup after every test */
}

TEST(ot_app_pair, GivenNullDeviceList_WhenCallingDeviceAdd_ThenReturnError)
{
    int8_t result;
    // otapp_pair_DeviceAdd(otapp_pair_getHandle(), &deviceNameFull_0, &ipAddr);
    result = otapp_pair_DeviceAdd(NULL, deviceNameFull_0, &ipAddr);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair, GivenNullDeviceNameFull_WhenCallingDeviceAdd_ThenReturnError)
{
    int8_t result;
    result = otapp_pair_DeviceAdd(otapp_pair_getHandle(), NULL, &ipAddr);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair, GivenNullIpAddr_WhenCallingDeviceAdd_ThenReturnError)
{
    int8_t result;
    result = otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_0, NULL);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair, GivenTwiceSameDeviceName_WhenCallingDeviceAdd_ThenReturnError)
{
    int8_t result;
             otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_0, &ipAddr);
    result = otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_0, &ipAddr);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_DEVICE_NAME_EXIST, result);   
}

TEST(ot_app_pair, GivenTooLengthDeviceName_WhenCallingDeviceAdd_ThenReturnError)
{
    int8_t result;
    result = otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_TooLength, &ipAddr);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_DEVICE_NAME_TO_LONG, result);   
}

TEST(ot_app_pair, GivenTrueVariables_WhenCallingDeviceAdd_ThenReturnIndex0)
{
    int8_t result;
    result = otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_0, &ipAddr);

    TEST_ASSERT_EQUAL(UT_OAP_RETURN_INDEX_0, result);   
}

TEST(ot_app_pair, GivenMaxLengthDeviceName_WhenCallingDeviceAdd_ThenReturnIndex0)
{
    int8_t result;
    result = otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_MaxLength10, &ipAddr);

    TEST_ASSERT_EQUAL(UT_OAP_RETURN_INDEX_0, result);   
}

TEST(ot_app_pair, GivenMaxDevices_WhenCallingDeviceAdd_ThenReturnIndex9)
{
    int8_t result;
    
    result =  ut_oap_deviceAddFullFill();

    TEST_ASSERT_EQUAL(UT_OAP_RETURN_INDEX_9, result);   
}

TEST(ot_app_pair, GivenMaxDevicesPlus1_WhenCallingDeviceAdd_ThenReturnError)
{
    int8_t result;
    ut_oap_deviceAddFullFill();
    result = otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_MaxLength10, &ipAddr);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_DEVICE_NO_SPACE, result);   
}