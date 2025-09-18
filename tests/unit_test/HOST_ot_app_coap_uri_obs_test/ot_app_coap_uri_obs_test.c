#include "unity_fixture.h"
#include "ot_app_coap_uri_obs.h"

#define TEST_OBS_LIST_INDEX_0           0
#define TEST_OBS_LIST_INDEX_MAX         (OAC_URI_OBS_SUBSCRIBERS_MAX_NUM - 1)
#define TEST_OBS_LIST_INDEX_OVERFLOW    OAC_URI_OBS_SUBSCRIBERS_MAX_NUM

#define TEST_OBS_HANDLE oac_uri_obs_getSubListHandle()

void test_obs_fillTake();

TEST_GROUP(ot_app_coap_uri_obs);

TEST_SETUP(ot_app_coap_uri_obs)
{
    /* Init before every test */
    oac_uri_obs_deleteAll(TEST_OBS_HANDLE);    
}

TEST_TEAR_DOWN(ot_app_coap_uri_obs)
{
    /* Cleanup after every test */
}

void test_obs_fillTake()
{
    uint8_t i = 0;

    while (oac_uri_obs_spaceTake(TEST_OBS_HANDLE, i) == OAC_URI_OBS_OK)
    {
       i++;
    }    
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

TEST(ot_app_coap_uri_obs, GivenTrueIndexArg_WhenCallingSpaceTake_ThenOK)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_OK, result_);
}

TEST(ot_app_coap_uri_obs, GivenMaxTrueIndexArg_WhenCallingSpaceTake_ThenOK)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_MAX);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_OK, result_);
}

// deleteAll()
TEST(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingdeleteAll_ThenError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_deleteAll(NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingdeleteAll_ThenOk)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_deleteAll(TEST_OBS_HANDLE);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_OK, result_);
}

// spaceIsFree()
TEST(ot_app_coap_uri_obs, GivenNullArg_WhenCallingSpaceIsFree_ThenError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceIsFree(NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSpaceIsFree_ThenOk)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceIsFree(TEST_OBS_HANDLE);
    TEST_ASSERT_EQUAL(TEST_OBS_LIST_INDEX_0, result_);
}

TEST(ot_app_coap_uri_obs, GivenFillList_WhenCallingSpaceIsFree_ThenError)
{
    oacu_result_t result_;
    test_obs_fillTake();
    result_ = oac_uri_obs_spaceIsFree(TEST_OBS_HANDLE);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_LIST_FULL, result_);
}