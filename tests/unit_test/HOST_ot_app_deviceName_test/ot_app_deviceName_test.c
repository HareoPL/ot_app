#include "unity_fixture.h"
#include "ot_app_deviceName.h"


#define UT_DN_OK_DEVICE_TYPE_0                  OTAPP_CONTROL_PANEL

#define UT_DN_BAD_DEVICE_TYPE_0                 OTAPP_NO_DEVICE_TYPE
#define UT_DN_BAD_DEVICE_TYPE_OUTSIDE_SCOPE     OTAPP_END_OF_DEVICE_TYPE

static char *deviceName_0 = {"device1"};
static char *deviceName_max_10_byte = {"dMax10byte"};
static char *deviceName_to_long = {"name11_byte"};

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

    // otapp_deviceNameSet(deviceName_max_10_byte, UT_DN_OK_DEVICE_TYPE_0);
    devNameFull = otapp_deviceNameFullGet();
    TEST_ASSERT_EQUAL_STRING(NULL, devNameFull);
}