#include "unity_fixture.h"

TEST_GROUP_RUNNER(ot_app_coap_uri_obs)
{
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenNullArg_WhenCallingSpaceTake_ThenError);
   RUN_TEST_CASE(ot_app_coap_uri_obs, GivenOverflowIndexArg_WhenCallingSpaceTake_ThenError);

}
