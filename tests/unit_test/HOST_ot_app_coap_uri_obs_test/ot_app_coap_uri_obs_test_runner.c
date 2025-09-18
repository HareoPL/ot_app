#include "unity_fixture.h"

TEST_GROUP_RUNNER(ot_app_coap_uri_obs)
{
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

}
