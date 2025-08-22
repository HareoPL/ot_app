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

   RUN_TEST_CASE(ot_app_pair, GivenNullDeviceList_WhenCallingDeviceDelete_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenNullDeviceName_WhenCallingDeviceDelete_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenTrueVariable_WhenCallingDeviceDelete_ThenReturn3);
   RUN_TEST_CASE(ot_app_pair, GivenSameDeviceNameTwice_WhenCallingDeviceDelete_ThenReturnError);

   RUN_TEST_CASE(ot_app_pair, GivenNullDeviceList_WhenCallingDeviceIndexGet_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenNullDeviceName_WhenCallingDeviceIndexGet_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenCorrectVariable_WhenCallingDeviceIndexGet_ThenReturn9);
   RUN_TEST_CASE(ot_app_pair, GivenNoExistDeviceName_WhenCallingDeviceIndexGet_ThenReturnError);

   RUN_TEST_CASE(ot_app_pair, GivenNullPairDeviceList_WhenCallingDeviceNameGet_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenBadIndexDevice_WhenCallingDeviceNameGet_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenNotExistIndexDevice_WhenCallingDeviceNameGet_ThenReturnNull);
   RUN_TEST_CASE(ot_app_pair, GivenExistIndexDevice_WhenCallingDeviceNameGet_ThenReturnTrueString);
}

TEST_GROUP_RUNNER(ot_app_pair_UriIndex)
{
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenNullDeviceList_WhenCallingUriIndexAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenNullDeviceName_WhenCallingUriIndexAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenBadUri_WhenCallingUriIndexAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenTrueUri_WhenCallingUriIndexAdd_ThenReturn0);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenMaxUri_WhenCallingUriIndexAdd_ThenReturn19);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenMaxUriPlus1_WhenCallingUriIndexAdd_ThenReturnError);

   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenNullDeviceList_WhenCallingDeviceUriIndexGet_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenBadIndexDevice_WhenCallingDeviceUriIndexGet_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenBadIndexUri_WhenCallingDeviceUriIndexGet_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenCorrectArguments_WhenCallingDeviceUriIndexGet_ThenReturn1);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenIndexUriMax_WhenCallingDeviceUriIndexGet_ThenReturn2);
}
