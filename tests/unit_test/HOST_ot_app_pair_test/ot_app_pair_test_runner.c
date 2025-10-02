#include "unity_fixture.h"

TEST_GROUP_RUNNER(ot_app_pair)
{
   /* Test cases to run */
   RUN_TEST_CASE(ot_app_pair, GivenNullDeviceList_WhenCallingDeviceAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenNullDeviceNameFull_WhenCallingDeviceAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenNullIpAddr_WhenCallingDeviceAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenTwiceSameDeviceName_WhenCallingDeviceAdd_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenTwiceWithDifferentIp_WhenCallingDeviceAdd_ThenReturnOK);
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
   
   RUN_TEST_CASE(ot_app_pair, GivenNullDeviceList_WhenCallingipAddressIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenDeletedDeviceIndex_WhenCallingipAddressIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenIncorrectDeviceIndex_WhenCallingipAddressIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenNullIpAddress_WhenCallingipAddressIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenToShortIpAddress_WhenCallingipAddressIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenNotSameIpAddr_WhenCallingipAddressIsSame_ThenReturn0);
   RUN_TEST_CASE(ot_app_pair, GivenSameIpAddr_WhenCallingipAddressIsSame_ThenReturn1);

   RUN_TEST_CASE(ot_app_pair, GivenNullDeviceList_WhenCallingIpAddressUpdate_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenDeletedDeviceIndex_WhenCallingIpAddressUpdate_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenIncorrectDeviceIndex_WhenCallingIpAddressUpdate_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenNullIpAddress_WhenCallingIpAddressUpdate_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair, GivenSameIpAddress_WhenCallingIpAddressUpdate_ThenReturn0);
   RUN_TEST_CASE(ot_app_pair, GivenNewIpAddress_WhenCallingIpAddressUpdate_ThenReturn1);
   RUN_TEST_CASE(ot_app_pair, GivenNewIpAddress_WhenCallingIpAddressUpdateAndIpAddressIsSame_ThenReturn1);

}

TEST_GROUP_RUNNER(ot_app_pair_UriIndex)
{
      // otapp_pair_uriResourcesCreate
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenNullArg_WhenCallingUriResourcesCreate_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenZeroUriSize_WhenCallingUriResourcesCreate_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenNullBufferSize_WhenCallingUriResourcesCreate_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenTruArgsSize1_WhenCallingUriResourcesCreate_ThenReturnOK);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenTruArgsSize3_WhenCallingUriResourcesCreate_ThenReturnOK);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenOvervflowSize_WhenCallingUriResourcesCreate_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenNullResultArg_WhenCallingUriResourcesCreate_ThenReturnError);

   // otapp_pair_uriParseMessage
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenNullInBuffer_WhenCallinguriParseMessage_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenZeroBufSize_WhenCallinguriParseMessage_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenNullOutParsedDataSize_WhenCallinguriParseMessage_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenTrueArgsSize1_WhenCallinguriParseMessage_ThenReturnOK);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenTrueArgsSize3_WhenCallinguriParseMessage_ThenReturnOK);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenTrueArgsSizeMax_WhenCallinguriParseMessage_ThenReturnOK);
   RUN_TEST_CASE(ot_app_pair_UriIndex, GivenOverflowSize_WhenCallinguriParseMessage_ThenReturnError);
}

TEST_GROUP_RUNNER(ot_app_pair_rtos)
{
   
   RUN_TEST_CASE(ot_app_pair_rtos, GivenInitQueue_WhenCalled_ThenReturnOK);
   RUN_TEST_CASE(ot_app_pair_rtos, GivenInitTask_WhenCalled_ThenReturnOK);
   RUN_TEST_CASE(ot_app_pair_rtos, GivenNullPtr_WhenCallingAddToQueue_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_rtos, GivenTrueQueueItem_WhenCallingAddToQueue_ThenReturnOK);
   RUN_TEST_CASE(ot_app_pair_rtos, GivenMaxQueueItems_WhenCallingAddToQueue_ThenReturnOK);
   RUN_TEST_CASE(ot_app_pair_rtos, GivenMaxPlus1QueueItems_WhenCallingAddToQueue_ThenReturnError);
   RUN_TEST_CASE(ot_app_pair_rtos, GivenMaxQueueItems_WhenCallingRtosTask_ThenReturnItemQuantity9);
   RUN_TEST_CASE(ot_app_pair_rtos, GivenMaxQueueItems_WhenCallingRtosTaskMaxTimes_ThenReturnItemQuantity0);
   RUN_TEST_CASE(ot_app_pair_rtos, GivenMaxQueueItems_WhenCallingRtosTaskMaxTimesPlus1_ThenReturnItemQuantity0);

}