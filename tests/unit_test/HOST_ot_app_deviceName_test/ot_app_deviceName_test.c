#include "unity_fixture.h"
#include "ot_app_deviceName.h"


#define UT_DN_OK_DEVICE_TYPE_0                  OTAPP_CONTROL_PANEL

#define UT_DN_BAD_DEVICE_TYPE_0                 OTAPP_NO_DEVICE_TYPE
#define UT_DN_BAD_DEVICE_TYPE_OUTSIDE_SCOPE     OTAPP_END_OF_DEVICE_TYPE

static char *deviceName_0 = {"device1"};
static char *deviceName_max_10_byte = {"dMax10byte"};
static char *deviceName_to_long = {"name11_byte"};

static char *deviceNameFull_to_long = {"device1_1_588c81fffe301ea4_too_long"};
static char *deviceNameFull_not_same = {"device1_1_588c81fffe301ea4"};
static char *deviceNameFull_devName0_type0_fakeAddr = {"device1_1_0011223344556677"};

TEST_GROUP(ot_app_deviceName);

TEST_SETUP(ot_app_deviceName)
{
    /* Init before every test */
    otapp_deviceNameDelete();
}

TEST_TEAR_DOWN(ot_app_deviceName)
{
    /* Cleanup after every test */
}

TEST(ot_app_deviceName, GivenNULLdevName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(NULL, UT_DN_OK_DEVICE_TYPE_0);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenCorrectDevName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(deviceName_0, UT_DN_OK_DEVICE_TYPE_0);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_OK, result);
}

TEST(ot_app_deviceName, GivenIncorrectDeviceName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(deviceName_0, UT_DN_BAD_DEVICE_TYPE_0);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenOutsideScopeDeviceName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(deviceName_0, UT_DN_BAD_DEVICE_TYPE_OUTSIDE_SCOPE);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenCorrectDeviceName_WhenIsCallingDeviceNameSet_ThenReturnOK)
{
    int8_t result;

    result = otapp_deviceNameSet(deviceName_0, UT_DN_OK_DEVICE_TYPE_0);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_OK, result);
}

TEST(ot_app_deviceName, GivenTooLongDevName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(deviceName_to_long, UT_DN_OK_DEVICE_TYPE_0);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_TOO_LONG, result);
}

TEST(ot_app_deviceName, GivenMaxLengthDevName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(deviceName_max_10_byte, UT_DN_OK_DEVICE_TYPE_0);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_OK, result);
}

// deviceNameFullGet
TEST(ot_app_deviceName, GivenNoCallingDeviceNameSet_WhenIsCallingDeviceNameFullGet_ThenReturnError)
{
    const char *devNameFull;

    devNameFull = otapp_deviceNameFullGet();
    TEST_ASSERT_EQUAL_STRING(NULL, devNameFull);
}

TEST(ot_app_deviceName, GivenCallingDeviceNameSet_WhenIsCallingDeviceNameFullGet_ThenReturnOK)
{
    const char *devNameFull;

    otapp_deviceNameSet(deviceName_0, UT_DN_OK_DEVICE_TYPE_0);
    devNameFull = otapp_deviceNameFullGet();
    TEST_ASSERT_EQUAL_STRING(deviceNameFull_devName0_type0_fakeAddr, devNameFull);
}

// deviceNameFullIsSame
TEST(ot_app_deviceName, GivenNullArgument_WhenIsCallingdeviceNameFullIsSame_ThenReturnError)
{
    int8_t result;
    result = otapp_deviceNameFullIsSame(NULL);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenTooLongDevNameFull_WhenIsCallingdeviceNameFullIsSame_ThenReturnError)
{
    int8_t result;
    result = otapp_deviceNameFullIsSame(deviceNameFull_to_long);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_TOO_LONG, result);
}

TEST(ot_app_deviceName, GivenNotSameDevNameFull_WhenIsCallingdeviceNameFullIsSame_ThenReturnNOT)
{
    int8_t result;
    otapp_deviceNameSet(deviceName_0, UT_DN_OK_DEVICE_TYPE_0);    
    result = otapp_deviceNameFullIsSame(deviceNameFull_not_same);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_IS_NOT, result);
}

TEST(ot_app_deviceName, GivenSameDevNameFull_WhenIsCallingdeviceNameFullIsSame_ThenReturnYES)
{
    int8_t result;
    otapp_deviceNameSet(deviceName_0, UT_DN_OK_DEVICE_TYPE_0);    
    result = otapp_deviceNameFullIsSame(deviceNameFull_devName0_type0_fakeAddr);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_IS, result);
}