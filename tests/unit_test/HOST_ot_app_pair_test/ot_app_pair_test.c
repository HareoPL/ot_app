#include "unity_fixture.h"
#include "ot_app_pair.h"

#define UT_OAP_RETURN_INDEX_0 (0)
#define UT_OAP_RETURN_INDEX_5 (5)
#define UT_OAP_RETURN_INDEX_9 (9)

#define UT_OAP_IS_SAME (OTAPP_PAIR_IS)
#define UT_OAP_IS_NOT_SAME (OTAPP_PAIR_IS_NOT)

#define UT_OAP_DEVICE_INDEX_0 (0)
#define UT_OAP_DEVICE_INDEX_1 (1)
#define UT_OAP_DEVICE_INDEX_BAD (OTAPP_PAIR_DEVICES_MAX)

#define UT_OAP_URI_TABLE_1      (OTAPP_URI_WELL_KNOWN_CORE)
#define UT_OAP_URI_TABLE_2      (OTAPP_URI_TEST)
#define UT_OAP_URI_TABLE_20     (OTAPP_URI_MOCK_20)

#define UT_OAP_DEVICE_NAME_FULL_3 (3)
#define UT_OAP_DEVICE_NAME_FULL_9 (9)

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
static char *deviceNameFull_NO_EXIST = {"device1_1_no_exist"};
static char *deviceNameFull_MaxLength10 = {"device1_1_588c81fffe301ea4___31"};
static char *deviceNameFull_TooLength = {"device1_1_588c81fffe301ea9____32"};

static otIp6Address ipAddr_ok_1 = {
    .mFields.m8 = {0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
                   0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x34}
};

static otIp6Address ipAddr_ok_2 = {
    .mFields.m8 = {0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73,  0x00,
                   0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x34}
};

static otIp6Address ipAddr_bad_to_short_lenght = {
    .mFields.m8 = {0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x34,
                   0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x2e}
};

static otIp6Address *ipAddr_same = &ipAddr_ok_1;
static otIp6Address *ipAddr_new = &ipAddr_ok_2;

static void ut_oap_DeviceUriIndexAddFillAll(otapp_coap_uriIndex_t uriIndex)
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
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_0, &ipAddr_ok_1);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_1, &ipAddr_ok_1);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_2, &ipAddr_ok_1);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_3, &ipAddr_ok_1);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_4, &ipAddr_ok_1);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_5, &ipAddr_ok_1);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_6, &ipAddr_ok_1);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_7, &ipAddr_ok_1);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_8, &ipAddr_ok_1);
    return otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_9, &ipAddr_ok_1);
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
    result = otapp_pair_DeviceAdd(NULL, deviceNameFull_0, &ipAddr_ok_1);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair, GivenNullDeviceNameFull_WhenCallingDeviceAdd_ThenReturnError)
{
    int8_t result;
    result = otapp_pair_DeviceAdd(otapp_pair_getHandle(), NULL, &ipAddr_ok_1);

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
             otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_0, &ipAddr_ok_1);
    result = otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_0, &ipAddr_ok_1);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_NO_NEED_UPDATE, result);   
}
TEST(ot_app_pair, GivenTwiceWithDifferentIp_WhenCallingDeviceAdd_ThenReturnOK)
{
    otIp6Address *new_ip;
    int8_t devId;

    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_0, &ipAddr_ok_1);
    devId = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_0);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_0, &ipAddr_ok_2);

    new_ip = otapp_pair_ipAddressGet(otapp_pair_getHandle(), devId);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(&ipAddr_ok_2, new_ip, sizeof(otIp6Address));   
}

TEST(ot_app_pair, GivenTooLengthDeviceName_WhenCallingDeviceAdd_ThenReturnError)
{
    int8_t result;
    result = otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_TooLength, &ipAddr_ok_1);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_DEVICE_NAME_TO_LONG, result);   
}

TEST(ot_app_pair, GivenTrueVariables_WhenCallingDeviceAdd_ThenReturnIndex0)
{
    int8_t result;
    result = otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_0, &ipAddr_ok_1);

    TEST_ASSERT_EQUAL(UT_OAP_RETURN_INDEX_0, result);   
}

TEST(ot_app_pair, GivenMaxLengthDeviceName_WhenCallingDeviceAdd_ThenReturnIndex0)
{
    int8_t result;
    result = otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_MaxLength10, &ipAddr_ok_1);

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
    result = otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_MaxLength10, &ipAddr_ok_1);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_DEVICE_NO_SPACE, result);   
}

TEST(ot_app_pair, GivenNullDeviceList_WhenCallingDeviceDelete_ThenReturnError)
{
    int8_t result;
    result = otapp_pair_DeviceDelete(NULL, deviceNameFull_0);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair, GivenNullDeviceName_WhenCallingDeviceDelete_ThenReturnError)
{
    int8_t result;
    result = otapp_pair_DeviceDelete(otapp_pair_getHandle(), NULL);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair, GivenTrueVariable_WhenCallingDeviceDelete_ThenReturn3)
{
    int8_t result;
    ut_oap_deviceAddFullFill();
    ut_oap_DeviceUriIndexAddFillAll(UT_OAP_URI_TABLE_2);

    result = otapp_pair_DeviceDelete(otapp_pair_getHandle(), deviceNameFull_3);

    TEST_ASSERT_EQUAL(UT_OAP_DEVICE_NAME_FULL_3, result);   
}

TEST(ot_app_pair, GivenSameDeviceNameTwice_WhenCallingDeviceDelete_ThenReturnError)
{
    int8_t result;
    ut_oap_deviceAddFullFill();
    ut_oap_DeviceUriIndexAddFillAll(UT_OAP_URI_TABLE_2);

    otapp_pair_DeviceDelete(otapp_pair_getHandle(), deviceNameFull_3);
    result = otapp_pair_DeviceDelete(otapp_pair_getHandle(), deviceNameFull_3);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_NO_EXIST, result);   
}

TEST(ot_app_pair, GivenNullDeviceList_WhenCallingDeviceIndexGet_ThenReturnError)
{
    int8_t result;
 
    ut_oap_deviceAddFullFill();
    result = otapp_pair_DeviceIndexGet(NULL, deviceNameFull_9);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair, GivenNullDeviceName_WhenCallingDeviceIndexGet_ThenReturnError)
{
    int8_t result;
 
    ut_oap_deviceAddFullFill();
    result = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), NULL);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);   
}

TEST(ot_app_pair, GivenCorrectVariable_WhenCallingDeviceIndexGet_ThenReturn9)
{
    int8_t result;
 
    ut_oap_deviceAddFullFill();
    result = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_9);

    TEST_ASSERT_EQUAL(UT_OAP_RETURN_INDEX_9, result);   
}

TEST(ot_app_pair, GivenNoExistDeviceName_WhenCallingDeviceIndexGet_ThenReturnError)
{
    int8_t result;
 
    ut_oap_deviceAddFullFill();
    result = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_NO_EXIST);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_NO_EXIST, result);   
}

////////////////////////////
// DeviceNameGet()
TEST(ot_app_pair, GivenNullPairDeviceList_WhenCallingDeviceNameGet_ThenReturnError)
{
    char *result;   

    ut_oap_deviceAddFullFill();

    result = otapp_pair_DeviceNameGet(NULL, UT_OAP_DEVICE_INDEX_1);

    TEST_ASSERT_EQUAL(NULL, result);
}

TEST(ot_app_pair, GivenBadIndexDevice_WhenCallingDeviceNameGet_ThenReturnError)
{
    char *result;   

    ut_oap_deviceAddFullFill();

    result = otapp_pair_DeviceNameGet(otapp_pair_getHandle(), UT_OAP_DEVICE_INDEX_BAD);

    TEST_ASSERT_EQUAL(NULL, result);
}

TEST(ot_app_pair, GivenNotExistIndexDevice_WhenCallingDeviceNameGet_ThenReturnNull)
{
    char *result;   
    int8_t deletedIndex;

    ut_oap_deviceAddFullFill();

    deletedIndex = otapp_pair_DeviceDelete(otapp_pair_getHandle(), deviceNameFull_5);
    result = otapp_pair_DeviceNameGet(otapp_pair_getHandle(), deletedIndex);

    TEST_ASSERT_EQUAL(NULL, result);
}

TEST(ot_app_pair, GivenExistIndexDevice_WhenCallingDeviceNameGet_ThenReturnTrueString)
{
    static char *result;   
    int8_t deviceIndex;  

    ut_oap_deviceAddFullFill();

    deviceIndex = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_5);
    result = otapp_pair_DeviceNameGet(otapp_pair_getHandle(), deviceIndex);

    TEST_ASSERT_EQUAL_STRING(deviceNameFull_5, result);
}

////////////////////////////
// otapp_pair_ipAddress()
TEST(ot_app_pair, GivenNullDeviceList_WhenCallingipAddressIsSame_ThenReturnError)
{
    int8_t result;   
    int8_t deviceIndex;  

    ut_oap_deviceAddFullFill();

    deviceIndex = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_5);
    result = otapp_pair_ipAddressIsSame(NULL, deviceIndex, &ipAddr_ok_2);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair, GivenDeletedDeviceIndex_WhenCallingipAddressIsSame_ThenReturnError)
{
    int8_t result;   
    int8_t deletedIndex;  

    ut_oap_deviceAddFullFill();

    deletedIndex = otapp_pair_DeviceDelete(otapp_pair_getHandle(), deviceNameFull_5);
    result = otapp_pair_ipAddressIsSame(otapp_pair_getHandle(), deletedIndex, &ipAddr_ok_2);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_NO_EXIST, result);
}

TEST(ot_app_pair, GivenIncorrectDeviceIndex_WhenCallingipAddressIsSame_ThenReturnError)
{
    int8_t result;   

    ut_oap_deviceAddFullFill();

    result = otapp_pair_ipAddressIsSame(otapp_pair_getHandle(), UT_OAP_DEVICE_INDEX_BAD, &ipAddr_ok_2);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair, GivenNullIpAddress_WhenCallingipAddressIsSame_ThenReturnError)
{
    int8_t result;   
    int8_t deviceIndex;   

    ut_oap_deviceAddFullFill();

    deviceIndex = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_5);
    result = otapp_pair_ipAddressIsSame(otapp_pair_getHandle(), deviceIndex, NULL);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair, GivenToShortIpAddress_WhenCallingipAddressIsSame_ThenReturnError)
{
    int8_t result;   
    int8_t deviceIndex;   

    ut_oap_deviceAddFullFill();

    deviceIndex = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_5);
    result = otapp_pair_ipAddressIsSame(otapp_pair_getHandle(), deviceIndex, &ipAddr_bad_to_short_lenght);

    TEST_ASSERT_EQUAL(UT_OAP_IS_NOT_SAME, result);
}

TEST(ot_app_pair, GivenNotSameIpAddr_WhenCallingipAddressIsSame_ThenReturn0)
{
    int8_t result;   
    int8_t deviceIndex;  

    ut_oap_deviceAddFullFill();

    deviceIndex = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_5);
    result = otapp_pair_ipAddressIsSame(otapp_pair_getHandle(), deviceIndex, &ipAddr_ok_2);

    TEST_ASSERT_EQUAL(UT_OAP_IS_NOT_SAME, result);
}

TEST(ot_app_pair, GivenSameIpAddr_WhenCallingipAddressIsSame_ThenReturn1)
{
    int8_t result;   
    int8_t deviceIndex;  

    ut_oap_deviceAddFullFill();

    deviceIndex = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_5);
    result = otapp_pair_ipAddressIsSame(otapp_pair_getHandle(), deviceIndex, &ipAddr_ok_1);

    TEST_ASSERT_EQUAL(UT_OAP_IS_SAME, result);
}

TEST(ot_app_pair, GivenNullDeviceList_WhenCallingIpAddressUpdate_ThenReturnError)
{
    int8_t result;   
    int8_t deviceIndex;  

    ut_oap_deviceAddFullFill();

    deviceIndex = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_5);
    result = otapp_pair_ipAddressUpdate(NULL, deviceIndex, ipAddr_new);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair, GivenDeletedDeviceIndex_WhenCallingIpAddressUpdate_ThenReturnError)
{
    int8_t result;   
    int8_t deletedIndex;  

    ut_oap_deviceAddFullFill();

    deletedIndex = otapp_pair_DeviceDelete(otapp_pair_getHandle(), deviceNameFull_5);
    result = otapp_pair_ipAddressUpdate(otapp_pair_getHandle(), deletedIndex, ipAddr_new);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_NO_EXIST, result);
}

TEST(ot_app_pair, GivenIncorrectDeviceIndex_WhenCallingIpAddressUpdate_ThenReturnError)
{
    int8_t result;   

    ut_oap_deviceAddFullFill();

    result = otapp_pair_ipAddressUpdate(otapp_pair_getHandle(), UT_OAP_DEVICE_INDEX_BAD, ipAddr_new);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair, GivenNullIpAddress_WhenCallingIpAddressUpdate_ThenReturnError)
{
    int8_t result;   
    int8_t deviceIndex;   

    ut_oap_deviceAddFullFill();

    deviceIndex = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_5);
    result = otapp_pair_ipAddressUpdate(otapp_pair_getHandle(), deviceIndex, NULL);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair, GivenSameIpAddress_WhenCallingIpAddressUpdate_ThenReturn0)
{
    int8_t result;   
    int8_t deviceIndex;   

    ut_oap_deviceAddFullFill();

    deviceIndex = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_5);
    result = otapp_pair_ipAddressUpdate(otapp_pair_getHandle(), deviceIndex, ipAddr_same);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_NO_NEED_UPDATE , result);
}

TEST(ot_app_pair, GivenNewIpAddress_WhenCallingIpAddressUpdate_ThenReturn1)
{
    int8_t result;   
    int8_t deviceIndex;   

    ut_oap_deviceAddFullFill();

    deviceIndex = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_5);
    result = otapp_pair_ipAddressUpdate(otapp_pair_getHandle(), deviceIndex, ipAddr_new);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_UPDATED , result);
}

TEST(ot_app_pair, GivenNewIpAddress_WhenCallingIpAddressUpdateAndIpAddressIsSame_ThenReturn1)
{
    int8_t result;   
    int8_t deviceIndex;   

    ut_oap_deviceAddFullFill();

    deviceIndex = otapp_pair_DeviceIndexGet(otapp_pair_getHandle(), deviceNameFull_5);
    otapp_pair_ipAddressUpdate(otapp_pair_getHandle(), deviceIndex, ipAddr_new);
    result = otapp_pair_ipAddressIsSame(otapp_pair_getHandle(), deviceIndex, ipAddr_new);

    TEST_ASSERT_EQUAL(OTAPP_PAIR_IS , result);
}