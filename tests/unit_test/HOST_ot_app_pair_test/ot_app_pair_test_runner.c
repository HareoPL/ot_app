#include "unity_fixture.h"

TEST_GROUP_RUNNER(ot_app_pair)
{
   /* Test cases to run */
   RUN_TEST_CASE(ot_app_pair, GivenNullDeviceList_WhenCallingDeviceAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenNullDeviceNameFull_WhenCallingDeviceAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenNullIpAddr_WhenCallingDeviceAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenTwiceSameDeviceName_WhenCallingDeviceAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenTooLengthDeviceName_WhenCallingDeviceAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenTrueVariables_WhenCallingDeviceAdd_ThenReturnIndex0);
   RUN_TEST_CASE(ot_app_pair, GivenMaxLengthDeviceName_WhenCallingDeviceAdd_ThenReturnIndex0);
   RUN_TEST_CASE(ot_app_pair, GivenMaxDevices_WhenCallingDeviceAdd_ThenReturnIndex9);
   RUN_TEST_CASE(ot_app_pair, GivenMaxDevicesPlus1_WhenCallingDeviceAdd_ThenReturnError);
}

TEST_GROUP_RUNNER(ot_app_pair_UriIndex)
{
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenNullDeviceList_WhenCallingUriIndexAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenNullDeviceName_WhenCallingUriIndexAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenBadUri_WhenCallingUriIndexAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenTrueUri_WhenCallingUriIndexAdd_ThenReturn0);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenMaxUri_WhenCallingUriIndexAdd_ThenReturn19);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenMaxUriPlus1_WhenCallingUriIndexAdd_ThenReturnError);
}
