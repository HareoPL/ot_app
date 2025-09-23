#include "unity_fixture.h"

TEST_GROUP_RUNNER(ot_app_coap_uri_obs)
{
   // SpaceTake()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullArg_WhenCallingSpaceTake_ThenError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverflowIndexArg_WhenCallingSpaceTake_ThenError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueIndexArg_WhenCallingSpaceTake_ThenOK);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenMaxTrueIndexArg_WhenCallingSpaceTake_ThenOK);

   // deleteAll()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingdeleteAll_ThenError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingdeleteAll_ThenOk);

   // spaceIsFree()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullArg_WhenCallingSpaceIsFree_ThenError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSpaceIsFree_ThenOk);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenFillList_WhenCallingSpaceIsFree_ThenError);

   // spaceIsTaken()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSpaceIsTaken_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverflowIndexArg_WhenCallingSpaceIsTaken_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueIndexArg_WhenCallingSpaceIsTaken_ThenReturnOk_0);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenMaxIndexArg_WhenCallingSpaceIsTaken_ThenReturnOk_0);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTrueArg_WhenFillTakeAndCallingSpaceIsTaken_ThenReturnOk_1);

   // tokenIsSame()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullHandleArg_WhenCallingTokenIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullTokenToCheckArg_WhenCallingTokenIsSame_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenSameTokenToCheck_WhenCallingTokenIsSame_ThenReturnIs);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenDiffrentTokenToCheck_WhenCallingTokenIsSame_ThenReturnIsNot);

   // tokenIsExist()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullHandleArg_WhenCallingTokenIsExist_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullToken_WhenCallingTokenIsExist_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNotExistToken_WhenCallingTokenIsExist_ThenReturnIsNot);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenExistToken_WhenCallingTokenIsExist_ThenReturnIndex0);

   // subscribe()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullHandleArg_WhenCallingSubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullSubDataArg_WhenCallingSubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNewSub_WhenCallingSubscribe_ThenReturnOK);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenTwiceSameSub_WhenCallingSubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenSubStrWithoutIP_WhenCallingSubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenSubStrWithoutToken_WhenCallingSubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenSubStrWithoutUriIC_WhenCallingSubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenSubStrWithoutUriIS_WhenCallingSubscribe_ThenReturnError);

   // unsubscribe()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullHandleArg_WhenCallingUnubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullTokenArg_WhenCallingUnubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNotExistToken_WhenSubStrIsEmptyAndCallingUnubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNotExistToken_WhenSubStrIsNotEmptyCallingUnubscribe_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenExistToken_WhenCallingUnubscribe_ThenReturnOK);
   RUN_TEST_CASE(ot_app_coap_uri_obs, CheckUnsubscribedToken_WhenCallingUnubscribe_ThenReturnError);

   // notify()
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullHandleArg_WhenCallingNotify_ThenReturnError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenIncorrectUriArg_WhenCallingNotify_ThenReturnError);

}
