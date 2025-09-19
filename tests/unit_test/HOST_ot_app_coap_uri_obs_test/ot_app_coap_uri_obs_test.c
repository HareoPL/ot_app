#include "unity_fixture.h"
#include "ot_app_coap_uri_obs.h"
#include "mock_ip6.h"

#define TEST_OBS_LIST_INDEX_0           0
#define TEST_OBS_LIST_INDEX_MAX         (OAC_URI_OBS_SUBSCRIBERS_MAX_NUM - 1)
#define TEST_OBS_LIST_INDEX_OVERFLOW    OAC_URI_OBS_SUBSCRIBERS_MAX_NUM

#define TEST_OBS_RETURN_TRUE    1
#define TEST_OBS_RETURN_FALSE   0

#define TEST_OBS_HANDLE oac_uri_obs_getSubListHandle()

oacu_token_t test_obs_token_3Byte[] = {0xFA, 0x04, 0xB6};
oacu_token_t test_obs_token_4Byte[] = {0xFA, 0x04, 0xB6, 0xD1};
oacu_token_t test_obs_token_5Byte[] = {0xFA, 0x04, 0xB6, 0xD1, 0xF1};

static otIp6Address test_obs_ipAddr_ok_1 = {
    .mFields.m8 = {0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
                    0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x34}
};

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

// spaceIsTaken()
TEST(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSpaceIsTaken_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceIsTaken(NULL, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverflowIndexArg_WhenCallingSpaceIsTaken_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceIsTaken(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueIndexArg_WhenCallingSpaceIsTaken_ThenReturnOk_0)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceIsTaken(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(TEST_OBS_RETURN_FALSE, result_);
}

TEST(ot_app_coap_uri_obs, GivenMaxIndexArg_WhenCallingSpaceIsTaken_ThenReturnOk_0)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceIsTaken(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(TEST_OBS_RETURN_FALSE, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenFillTakeAndCallingSpaceIsTaken_ThenReturnOk_1)
{
    oacu_result_t result_;
    test_obs_fillTake();
    result_ = oac_uri_obs_spaceIsTaken(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(TEST_OBS_RETURN_TRUE, result_);

    result_ = oac_uri_obs_spaceIsTaken(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_MAX);
    TEST_ASSERT_EQUAL(TEST_OBS_RETURN_TRUE, result_);
}

// tokenIsSame()
TEST(ot_app_coap_uri_obs, GivenNullHandleArg_WhenCallingTokenIsSame_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_tokenIsSame(NULL, TEST_OBS_LIST_INDEX_0, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenNullTokenToCheckArg_WhenCallingTokenIsSame_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_tokenIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenSameTokenToCheck_WhenCallingTokenIsSame_ThenReturnIs)
{
    oacu_result_t result_;
    test_obs_fillListExampleData(TEST_OBS_HANDLE);
    result_ = oac_uri_obs_tokenIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, test_obs_token_4Byte);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_IS, result_);
}

TEST(ot_app_coap_uri_obs, GivenDiffrentTokenToCheck_WhenCallingTokenIsSame_ThenReturnIsNot)
{
    oacu_result_t result_;
    test_obs_fillListExampleData(TEST_OBS_HANDLE);
    result_ = oac_uri_obs_tokenIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, test_obs_token_3Byte);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_IS_NOT, result_);
}

