#include "unity_fixture.h"

TEST_GROUP_RUNNER(ot_app_deviceName)
{
   RUN_TEST_CASE(ot_app_deviceName, GivenNULLdevName_WhenIsCallingDeviceNameSet_ThenReturnError);
   RUN_TEST_CASE(ot_app_deviceName, GivenCorrectDevName_WhenIsCallingDeviceNameSet_ThenReturnError);
   RUN_TEST_CASE(ot_app_deviceName, GivenIncorrectDeviceName_WhenIsCallingDeviceNameSet_ThenReturnError);
   RUN_TEST_CASE(ot_app_deviceName, GivenOutsideScopeDeviceName_WhenIsCallingDeviceNameSet_ThenReturnError);
   RUN_TEST_CASE(ot_app_deviceName, GivenCorrectDeviceName_WhenIsCallingDeviceNameSet_ThenReturnOK);
   RUN_TEST_CASE(ot_app_deviceName, GivenTooLongDevName_WhenIsCallingDeviceNameSet_ThenReturnError);
   RUN_TEST_CASE(ot_app_deviceName, GivenMaxLengthDevName_WhenIsCallingDeviceNameSet_ThenReturnError);

   // deviceNameFullGet
   RUN_TEST_CASE(ot_app_deviceName, GivenNoCallingDeviceNameSet_WhenIsCallingDeviceNameFullGet_ThenReturnError);
}

