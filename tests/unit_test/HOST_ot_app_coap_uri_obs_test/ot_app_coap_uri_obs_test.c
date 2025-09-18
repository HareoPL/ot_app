#include "unity_fixture.h"
#include "ot_app_coap_uri_obs.h"

#define TEST_OBS_LIST_INDEX_0           0
#define TEST_OBS_LIST_INDEX_MAX         (OAC_URI_OBS_SUBSCRIBERS_MAX_NUM - 1)
#define TEST_OBS_LIST_INDEX_OVERFLOW    OAC_URI_OBS_SUBSCRIBERS_MAX_NUM

#define TEST_OBS_HANDLE oac_uri_obs_getSubListHandle()

TEST_GROUP(ot_app_coap_uri_obs);

TEST_SETUP(ot_app_coap_uri_obs)
{
    /* Init before every test */
}

TEST_TEAR_DOWN(ot_app_coap_uri_obs)
{
    /* Cleanup after every test */
}

// spaceTake()
TEST(ot_app_coap_uri_obs, GivenNullArg_WhenCallingSpaceTake_ThenError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceTake(NULL, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverflowIndexArg_WhenCallingSpaceTake_ThenError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}
