#include "unity_fixture.h"

TEST_GROUP_RUNNER(ot_app_deviceName)
{
   RUN_TEST_CASE(ot_app_deviceName, GivenNULLdevName_WhenIsCallingDeviceNameSet_ThenReturnError);
   RUN_TEST_CASE(ot_app_deviceName, GivenIncorrectDeviceName_WhenIsCallingDeviceNameSet_ThenReturnError);


}

