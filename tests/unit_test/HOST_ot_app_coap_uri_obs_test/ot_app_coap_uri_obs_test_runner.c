#include "unity_fixture.h"

TEST_GROUP_RUNNER(ot_app_coap_uri_obs)
{
   // oac_uri_obs_spaceDevNameTake()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullArg_WhenCallingSpaceDevNameTake_ThenError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverflowIndexArg_WhenCallingSpaceDevNameTake_ThenError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueIndexArg_WhenCallingSpaceDevNameTake_ThenOK);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenMaxTrueIndexArg_WhenCallingSpaceDevNameTake_ThenOK);

   // deleteAll()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingdeleteAll_ThenError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingdeleteAll_ThenOk);

   // spaceDevNameIsFree()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullArg_WhenCallingSpaceDevNameIsFree_ThenError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSpaceDevNameIsFree_ThenOk);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenFillList_WhenCallingSpaceDevNameIsFree_ThenError);

   // spaceDevNameTake()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSpaceDevNameTake_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverflowIndexArg_WhenCallingSpaceDevNameTake_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueIndexArg_WhenCallingSpaceDevNameTake_ThenReturnOk_0);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenMaxIndexArg_WhenCallingSpaceDevNameTake_ThenReturnOk_0);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenSpaceDevNameTakeAndCallingSpaceDevNameTake_ThenReturn_error);

   // spaceDevNameIsTaken
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSpaceDevNameIsTaken_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabDevIdArg_WhenCallingSpaceDevNameIsTaken_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSpaceDevNameIsTaken_ThenReturnOK);

   // spaceUriIsFree()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullArg_WhenCallingSpaceUriIsFree_ThenError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSpaceUriIsFree_ThenOk);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenFillList_WhenCallingSpaceUriIsFree_ThenError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabDevId_WhenCallingSpaceUriIsFree_ThenError);

   // spaceUriTake()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSpaceUriTake_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverflowIndexArg_WhenCallingSpaceUriTake_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueIndexArg_WhenCallingSpaceUriTake_ThenReturnOk_0);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenMaxIndexArg_WhenCallingSpaceUriTake_ThenReturnOk_0);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenSpaceUriTakeAndCallingSpaceUriTake_ThenReturn_error);

   // spaceUriIsTaken()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSpaceUriIsTaken_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabDevIdArg_WhenCallingSpaceUriIsTaken_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSpaceUriIsTaken_ThenReturn_0);

   // uriIsExist
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingUriIsExist_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabDevIdArg_WhenCallingUriIsExist_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArgWithoutFill_WhenCallingUriIsExist_ThenReturn_isNot);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArgWithFill_WhenCallingUriIsExist_ThenReturn_tabUriId0_4);

   // saveDeviceNameFull()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSaveDeviceNameFull_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNllNameArg_WhenCallingSaveDeviceNameFull_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabdevidArg_WhenCallingSaveDeviceNameFull_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSaveDeviceNameFull_ThenReturnOK);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTooLongNameArg_WhenCallingSaveDeviceNameFull_ThenReturnError);

   //saveIpAddr()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSaveIpAddr_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNllNameArg_WhenCallingSaveIpAddr_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabdevidArg_WhenCallingSaveIpAddr_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSaveIpAddr_ThenReturnOK);

   // saveUriIndex()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSaveUriIndex_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabdevidArg_WhenCallingSaveUriIndex_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabUriIdArg_WhenCallingSaveUriIndex_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSaveUriIndex_ThenReturnOK);

   // saveToken()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSaveToken_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullTokenArg_WhenCallingSaveToken_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabdevidArg_WhenCallingSaveToken_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabUriIdArg_WhenCallingSaveToken_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSaveToken_ThenReturnOK);

   // addNewDevice()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingAddNewDevice_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullNameArg_WhenCallingAddNewDevice_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullIpArg_WhenCallingAddNewDevice_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTooLongDevNameArg_WhenCallingAddNewDevice_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArgAndNoSpace_WhenCallingAddNewDevice_ThenReturn_LIST_FULL);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingAddNewDevice_ThenReturnOK);

   // addNewUri()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingAddNewUri_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullTokenArg_WhenCallingAddNewUri_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArgAndNoSpace_WhenCallingAddNewUri_ThenReturn_LIST_FULL);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabDevId_WhenCallingAddNewUri_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingAddNewUri_ThenReturnOK);

   // tokenIsSame()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullHandleArg_WhenCallingTokenIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullTokenToCheckArg_WhenCallingTokenIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabDevIdArg_WhenCallingTokenIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabUriIdArg_WhenCallingTokenIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenSameTokenToCheck_WhenCallingTokenIsSame_ThenReturnIs);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenDifferentTokenToCheck_WhenCallingTokenIsSame_ThenReturnIsNot);

   // tokenIsExist()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullHandleArg_WhenCallingTokenIsExist_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullToken_WhenCallingTokenIsExist_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNotExistToken_WhenCallingTokenIsExist_ThenReturnIsNot);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabDevIdArg_WhenCallingTokenIsExist_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenExistToken_WhenCallingTokenIsExist_ThenReturnIndex0);
   
   // ipAddrIsSame()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArgs_WhenCallingIpAddrIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverFlowTabDevIdArgs_WhenCallingIpAddrIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenSameIpdArgs_WhenCallingIpAddrIsSame_ThenReturn_IS);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenDifferentIpdArgs_WhenCallingIpAddrIsSame_ThenReturn_IS_NOT);

   // devNameFullIsSame()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArgs_WhenCallingDevNameFullIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverflowArgs_WhenCallingDevNameFullIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenToLongDevNameArgs_WhenCallingDevNameFullIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArgs_WhenCallingDevNameFullIsSame_ThenReturn_is);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenDifferentArgs_WhenCallingDevNameFullIsSame_ThenReturn_is_not);

   // devNameFullIsExist()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArgs_WhenCallingDevNameFullIsExist_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTooLongDevNameArgs_WhenCallingDevNameFullIsExist_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenExistDevNameArgs_WhenCallingDevNameFullIsExist_ThenReturn_is);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNotExistDevNameArgs_WhenCallingDevNameFullIsExist_ThenReturn_is_not);
   
   // subscribe()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullHandleArgS_WhenCallingSubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenBadUriIndexArg_WhenCallingSubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTooLongDeviceName_WhenCallingSubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueFirstDevArgs_WhenCallingSubscribe_ThenReturn_ok);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueTwiceFirstDevArgs_WhenCallingSubscribe_ThenReturn_ok);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueTwiceDifferentDevArgs_WhenCallingSubscribe_ThenReturn_ok);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueTwiceDeviceNameButDifferentToken_WhenCallingSubscribe_ThenReturn_ok);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueTwiceDeviceNameButDifferentUri_WhenCallingSubscribe_ThenReturn_ok);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueTwiceDeviceNameButDifferentIpAddr_WhenCallingSubscribe_ThenReturn_ok);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueTwiceDeviceNameButDifferentTokenAndIpAddr_WhenCallingSubscribe_ThenReturn_ok);

   // unsubscribe()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullArgs_WhenCallingUnubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNotExistDevName_WhenCallingUnubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNotExistToken_WhenCallingUnubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenExistToken_WhenCallingUnubscribe_ThenReturn_ok);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenExistTokenAndTwoDifferentUris_WhenCallingUnubscribe_ThenReturn_ok);

   // notify()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullHandleArg_WhenCallingNotify_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenIncorrectUriArg_WhenCallingNotify_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArgs_WhenEmptySubscribeListCallingNotify_ThenReturnOk);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArgs_WhenSubscribeListIsFullCallingNotify_ThenReturnOk);
   RUN_TEST_CASE(ot_app_coap_uri_obs, CheckNotify_GivenTrueArgs_WhenCallingNotify_ThenReturnOk);

   // parseMessage()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullArgs_WhenParseMessage_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenParseMessage_ThenReturnPtrToStract);


}
