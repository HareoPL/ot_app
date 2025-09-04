#include "unity_fixture.h"
#include "ot_app_deviceName.h"


#define UT_DN_BAD_DEVICE_TYPE_0     OTAPP_NO_DEVICE_TYPE
#define UT_DN_BAD_DEVICE_TYPE_MAX   OTAPP_END_OF_DEVICE_TYPE

static char *deviceName_0 = {"device1"};
static char *deviceName_max_10_byte = {"dMax10byte"};
static char *deviceName_to_long = {"name11_byte"};

TEST_GROUP(ot_app_deviceName);

TEST_SETUP(ot_app_deviceName)
{
    /* Init before every test */
}

TEST_TEAR_DOWN(ot_app_deviceName)
{
    /* Cleanup after every test */
}

TEST(ot_app_deviceName, GivenNULLdevName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(NULL, OTAPP_SWITCH);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenIncorrectDeviceName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(deviceName_0, UT_DN_BAD_DEVICE_TYPE_0);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}