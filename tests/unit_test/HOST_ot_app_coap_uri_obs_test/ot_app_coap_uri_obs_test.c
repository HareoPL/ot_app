#include "unity_fixture.h"
#include "ot_app_coap_uri_obs.h"
#include "mock_ip6.h"
#include "string.h"

#define TEST_OBS_LIST_INDEX_0           0
#define TEST_OBS_LIST_INDEX_MAX         (OAC_URI_OBS_SUBSCRIBERS_MAX_NUM - 1)
#define TEST_OBS_LIST_INDEX_OVERFLOW    OAC_URI_OBS_SUBSCRIBERS_MAX_NUM

#define TEST_OBS_URI_INDEX_INDEX_0           0
#define TEST_OBS_URI_INDEX_INDEX_MAX         (OAC_URI_OBS_PAIRED_URI_MAX - 1)
#define TEST_OBS_URI_INDEX_INDEX_OVERFLOW    OAC_URI_OBS_PAIRED_URI_MAX

#define TEST_OBS_RETURN_TRUE    OAC_URI_OBS_OK
#define TEST_OBS_RETURN_FALSE   OAC_URI_OBS_ERROR

#define TEST_OBS_URI_INDEX_1 1
#define TEST_OBS_URI_INDEX_2 2
#define TEST_OBS_URI_INDEX_3 3
#define TEST_OBS_URI_INDEX_4 4
#define TEST_OBS_URI_INDEX_5 5

#define TEST_OBS_HANDLE oac_uri_obs_getSubListHandle()

static uint8_t txRxBuffer[OAC_URI_OBS_TX_BUFFER_SIZE];

oacu_token_t test_obs_token_3Byte[] = {0xFA, 0x04, 0xB6};
oacu_token_t test_obs_token_4Byte[] = {0xFA, 0x04, 0xB6, 0xD1};
oacu_token_t test_obs_token_4Byte_2[] = {0x04, 0xB6, 0xD1, 0xFA};
oacu_token_t test_obs_token_5Byte[] = {0xFA, 0x04, 0xB6, 0xD1, 0xF1};

char *deviceNameFull_too_long_32 = "device1_1_588c81fffe301ea4000000";
char *deviceNameFull_Different = "device1_1_588c81fffe301ea5";

static oac_uri_observer_t test_obs_obsTrue={       
    .deviceNameFull = "device1_1_588c81fffe301ea4", 
    .takenPosition_dev = 1,
    .ipAddr.mFields.m8 = {
                    0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
                    0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x34},
    
    .uri[0].token = {
                    0xFA, 0x04, 0xB6, 0xD1},
    .uri[0].takenPosition_uri = 1,
    .uri[0].uriIndex = 2
};

static oac_uri_observer_t test_obs_obsTrue2={       
    .deviceNameFull = "device1_1_588c81fffe301ea2", 
    .takenPosition_dev = 1,
    .ipAddr.mFields.m8 = {
                    0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
                    0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x32},
    
    .uri[0].token = {
                    0xFA, 0x04, 0xB6, 0xD2},
    .uri[0].takenPosition_uri = 1,
    .uri[0].uriIndex = 3
};

otIp6Address ipAddr_different = {
    .mFields.m8= {
                    0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
                    0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x42},
};


oac_uri_dataPacket_t test_obs_dataPacket;
oac_uri_dataPacket_t test_obs_dataPacketOut;

void test_obs_fillTxBuffer(oac_uri_observer_t *subListHandle, uint8_t *data, uint16_t dataSize)
{
    // clear tx buffer
    memset(txRxBuffer, 0, sizeof(txRxBuffer)); 

    // copy token to tx buffer
    memcpy(txRxBuffer, subListHandle[0].uri[1].token, OAC_URI_OBS_TOKEN_LENGTH);
    
    // increase tx buffer ptr about token lenght
    // copy dataToNotify to tx buffer 
    memcpy(txRxBuffer + OAC_URI_OBS_TOKEN_LENGTH, data, dataSize);

}

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

oac_uri_dataPacket_t *test_obs_createDataPacket(oacu_token_t *token, uint8_t *buffer, uint16_t bufferSize)
{       
    memset(&test_obs_dataPacket, 0, sizeof(oac_uri_dataPacket_t));
    memcpy(&test_obs_dataPacket.token, token, OAC_URI_OBS_TOKEN_LENGTH);
    memcpy(&test_obs_dataPacket.buffer, buffer, bufferSize);
    
    return &test_obs_dataPacket;
}

void test_obs_fill_spaceDevNameTake()
{
    uint8_t i = 0;

    while (oac_uri_obs_spaceDevNameTake(TEST_OBS_HANDLE, i) == OAC_URI_OBS_OK)
    {
       i++;
    }    
}

void test_obs_fill_spaceUriTake()
{
    uint8_t i = 0;

    while (oac_uri_obs_spaceUriTake(TEST_OBS_HANDLE, 0, i) == OAC_URI_OBS_OK)
    {
       i++;
    }    
}

// oac_uri_obs_spaceDevNameTake()
TEST(ot_app_coap_uri_obs, GivenNullArg_WhenCallingSpaceDevNameTake_ThenError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceDevNameTake(NULL, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverflowIndexArg_WhenCallingSpaceDevNameTake_ThenError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceDevNameTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueIndexArg_WhenCallingSpaceDevNameTake_ThenOK)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceDevNameTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_OK, result_);
}

TEST(ot_app_coap_uri_obs, GivenMaxTrueIndexArg_WhenCallingSpaceDevNameTake_ThenOK)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceDevNameTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_MAX);
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
    oac_uri_observer_t *ptrList;
    uint8_t *ptr;
    uint16_t ptrListSize;
    
    test_obs_fillListExampleData(TEST_OBS_HANDLE);
    result_ = oac_uri_obs_deleteAll(TEST_OBS_HANDLE);
    
    ptrList = oac_uri_obs_getSubListHandle();   
    ptrListSize = sizeof(ptrList[0]);

    ptr = (uint8_t*)&ptrList[0];

    for (uint16_t i = 0; i < (OAC_URI_OBS_SUBSCRIBERS_MAX_NUM * ptrListSize); i++)
    {
        ptr++;
        TEST_ASSERT_EQUAL(0, *ptr);      
    }
    
    TEST_ASSERT_EQUAL(OAC_URI_OBS_OK, result_);
}

// spaceDevNameIsFree()
TEST(ot_app_coap_uri_obs, GivenNullArg_WhenCallingSpaceDevNameIsFree_ThenError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceDevNameIsFree(NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSpaceDevNameIsFree_ThenOk)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceDevNameIsFree(TEST_OBS_HANDLE);
    TEST_ASSERT_EQUAL(TEST_OBS_LIST_INDEX_0, result_);
}

TEST(ot_app_coap_uri_obs, GivenFillList_WhenCallingSpaceDevNameIsFree_ThenError)
{
    oacu_result_t result_;
    test_obs_fill_spaceDevNameTake();
    result_ = oac_uri_obs_spaceDevNameIsFree(TEST_OBS_HANDLE);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_LIST_FULL, result_);
}

// spaceDevNameTake()
TEST(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSpaceDevNameTake_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceDevNameTake(NULL, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverflowIndexArg_WhenCallingSpaceDevNameTake_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceDevNameTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueIndexArg_WhenCallingSpaceDevNameTake_ThenReturnOk_0)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceDevNameTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(TEST_OBS_RETURN_TRUE, result_);
}

TEST(ot_app_coap_uri_obs, GivenMaxIndexArg_WhenCallingSpaceDevNameTake_ThenReturnOk_0)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceDevNameTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_MAX);
    TEST_ASSERT_EQUAL(TEST_OBS_RETURN_TRUE, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenSpaceDevNameTakeAndCallingSpaceDevNameTake_ThenReturn_error)
{
    oacu_result_t result_;
    test_obs_fill_spaceDevNameTake();
    result_ = oac_uri_obs_spaceDevNameTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(TEST_OBS_RETURN_FALSE, result_);

    result_ = oac_uri_obs_spaceDevNameTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_MAX);
    TEST_ASSERT_EQUAL(TEST_OBS_RETURN_FALSE, result_);
}

// spaceDevNameIsTaken
TEST(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSpaceDevNameIsTaken_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceDevNameIsTaken(NULL, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabDevIdArg_WhenCallingSpaceDevNameIsTaken_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceDevNameIsTaken(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSpaceDevNameIsTaken_ThenReturnOK)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceDevNameIsTaken(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_MAX);
    TEST_ASSERT_EQUAL(0, result_);
}

// spaceUriIsFree()
TEST(ot_app_coap_uri_obs, GivenNullArg_WhenCallingSpaceUriIsFree_ThenError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceUriIsFree(NULL, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSpaceUriIsFree_ThenOk)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_spaceUriIsFree(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(TEST_OBS_LIST_INDEX_0, result_);
}

TEST(ot_app_coap_uri_obs, GivenFillList_WhenCallingSpaceUriIsFree_ThenError)
{
    oacu_result_t result_;
    test_obs_fill_spaceUriTake();
    result_ = oac_uri_obs_spaceUriIsFree(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_LIST_FULL, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabDevId_WhenCallingSpaceUriIsFree_ThenError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceUriIsFree(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

// spaceUriTake()
TEST(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSpaceUriTake_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceUriTake(NULL, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_0);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverflowIndexArg_WhenCallingSpaceUriTake_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceUriTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_OVERFLOW);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueIndexArg_WhenCallingSpaceUriTake_ThenReturnOk_0)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceUriTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_0);
    TEST_ASSERT_EQUAL(TEST_OBS_RETURN_TRUE, result_);
}

TEST(ot_app_coap_uri_obs, GivenMaxIndexArg_WhenCallingSpaceUriTake_ThenReturnOk_0)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceUriTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_MAX);
    TEST_ASSERT_EQUAL(TEST_OBS_RETURN_TRUE, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenSpaceUriTakeAndCallingSpaceUriTake_ThenReturn_error)
{
    oacu_result_t result_;
    test_obs_fill_spaceUriTake();
    result_ = oac_uri_obs_spaceUriTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_0);
    TEST_ASSERT_EQUAL(TEST_OBS_RETURN_FALSE, result_);

    result_ = oac_uri_obs_spaceUriTake(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_MAX);
    TEST_ASSERT_EQUAL(TEST_OBS_RETURN_FALSE, result_);
}

// spaceUriIsTaken()
TEST(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSpaceUriIsTaken_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceUriIsTaken(NULL, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_0);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabDevIdArg_WhenCallingSpaceUriIsTaken_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceUriIsTaken(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_OVERFLOW);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSpaceUriIsTaken_ThenReturn_0)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_spaceUriIsTaken(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_MAX, TEST_OBS_URI_INDEX_INDEX_MAX);
    TEST_ASSERT_EQUAL(0, result_);
}

// uriIsExist
TEST(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingUriIsExist_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_uriIsExist(NULL, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_2);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabDevIdArg_WhenCallingUriIsExist_ThenReturnError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_uriIsExist(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW, TEST_OBS_URI_INDEX_2);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArgWithoutFill_WhenCallingUriIsExist_ThenReturn_isNot)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_uriIsExist(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_2);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_IS_NOT, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArgWithFill_WhenCallingUriIsExist_ThenReturn_tabUriId0_4)
{
    oacu_result_t result_;

    test_obs_fillListExampleData(TEST_OBS_HANDLE);

    for (uint8_t i = 0; i < OAC_URI_OBS_PAIRED_URI_MAX; i++)
    {
        result_ = oac_uri_obs_uriIsExist(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, (i + 1));
        TEST_ASSERT_EQUAL(i, result_);
    }    
}

// saveDeviceNameFull()
TEST(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSaveDeviceNameFull_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_saveDeviceNameFull(NULL, TEST_OBS_LIST_INDEX_0, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenNllNameArg_WhenCallingSaveDeviceNameFull_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_saveDeviceNameFull(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabdevidArg_WhenCallingSaveDeviceNameFull_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_saveDeviceNameFull(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSaveDeviceNameFull_ThenReturnOK)
{
    oacu_result_t result_; 
    oac_uri_observer_t *obsList = oac_uri_obs_getSubListHandle();

    result_ = oac_uri_obs_saveDeviceNameFull(obsList, TEST_OBS_LIST_INDEX_0, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_OK, result_);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue.deviceNameFull, obsList[0].deviceNameFull, strlen(test_obs_obsTrue.deviceNameFull));
}

TEST(ot_app_coap_uri_obs, GivenTooLongNameArg_WhenCallingSaveDeviceNameFull_ThenReturnError)
{
    oacu_result_t result_; 
    oac_uri_observer_t *obsList = oac_uri_obs_getSubListHandle();

    result_ = oac_uri_obs_saveDeviceNameFull(obsList, TEST_OBS_LIST_INDEX_0, deviceNameFull_too_long_32); //lenght 32 + EOL
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

// saveIpAddr()
TEST(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSaveIpAddr_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_saveIpAddr(NULL, TEST_OBS_LIST_INDEX_0, &test_obs_obsTrue.ipAddr);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenNllNameArg_WhenCallingSaveIpAddr_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_saveIpAddr(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabdevidArg_WhenCallingSaveIpAddr_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_saveIpAddr(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW, &test_obs_obsTrue.ipAddr);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSaveIpAddr_ThenReturnOK)
{
    oacu_result_t result_; 
    oac_uri_observer_t *obsList = oac_uri_obs_getSubListHandle();

    result_ = oac_uri_obs_saveIpAddr(obsList, TEST_OBS_LIST_INDEX_0, &test_obs_obsTrue.ipAddr);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_OK, result_);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&test_obs_obsTrue.ipAddr, &obsList[0].ipAddr, OT_IP6_ADDRESS_SIZE);
}

// saveUriIndex()
TEST(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSaveUriIndex_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_saveUriIndex(NULL, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_0, TEST_OBS_URI_INDEX_1);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabdevidArg_WhenCallingSaveUriIndex_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_saveUriIndex(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW, TEST_OBS_URI_INDEX_INDEX_0, TEST_OBS_URI_INDEX_1);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabUriIdArg_WhenCallingSaveUriIndex_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_saveUriIndex(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_OVERFLOW, TEST_OBS_URI_INDEX_1);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSaveUriIndex_ThenReturnOK)
{
    oacu_result_t result_; 
    oac_uri_observer_t *obsList = oac_uri_obs_getSubListHandle();

    result_ = oac_uri_obs_saveUriIndex(obsList, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_0, TEST_OBS_URI_INDEX_1);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_OK, result_);
    TEST_ASSERT_EQUAL(TEST_OBS_URI_INDEX_1, obsList[0].uri[0].uriIndex);
}

// saveToken()
TEST(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingSaveToken_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_saveToken(NULL, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_0, test_obs_token_4Byte);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenNullTokenArg_WhenCallingSaveToken_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_saveToken(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_0, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabdevidArg_WhenCallingSaveToken_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_saveToken(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW, TEST_OBS_URI_INDEX_INDEX_0, test_obs_token_4Byte);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabUriIdArg_WhenCallingSaveToken_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_saveToken(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_OVERFLOW, test_obs_token_4Byte);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingSaveToken_ThenReturnOK)
{
    oacu_result_t result_; 
    oac_uri_observer_t *obsList = oac_uri_obs_getSubListHandle();

    result_ = oac_uri_obs_saveToken(obsList, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_0, test_obs_token_4Byte);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_OK, result_);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_token_4Byte, obsList[0].uri[0].token, OAC_URI_OBS_TOKEN_LENGTH);
}

// addNewDevice()
TEST(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingAddNewDevice_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_addNewDevice(NULL, test_obs_obsTrue.deviceNameFull, &test_obs_obsTrue.ipAddr);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenNullNameArg_WhenCallingAddNewDevice_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_addNewDevice(TEST_OBS_HANDLE, NULL, &test_obs_obsTrue.ipAddr);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenNullIpArg_WhenCallingAddNewDevice_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_addNewDevice(TEST_OBS_HANDLE, test_obs_obsTrue.deviceNameFull, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTooLongDevNameArg_WhenCallingAddNewDevice_ThenReturnError)
{
    oacu_result_t result_; 
    result_ = oac_uri_obs_addNewDevice(TEST_OBS_HANDLE, deviceNameFull_too_long_32, &test_obs_obsTrue.ipAddr);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArgAndNoSpace_WhenCallingAddNewDevice_ThenReturn_LIST_FULL)
{
    oacu_result_t result_; 
    test_obs_fill_spaceDevNameTake();
    result_ = oac_uri_obs_addNewDevice(TEST_OBS_HANDLE, test_obs_obsTrue.deviceNameFull, &test_obs_obsTrue.ipAddr);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_LIST_FULL, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingAddNewDevice_ThenReturnOK)
{
    oacu_result_t result_; 
    oac_uri_observer_t *obsList = oac_uri_obs_getSubListHandle();
    int8_t tabDevId_ = 0;
    result_ = oac_uri_obs_addNewDevice(obsList, test_obs_obsTrue.deviceNameFull, &test_obs_obsTrue.ipAddr);

    TEST_ASSERT_NOT_EQUAL(OAC_URI_OBS_ERROR, result_);
    TEST_ASSERT_EQUAL(tabDevId_, result_);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue.deviceNameFull, obsList[0].deviceNameFull, strlen(test_obs_obsTrue.deviceNameFull));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&test_obs_obsTrue.ipAddr, &obsList[0].ipAddr, OT_IP6_ADDRESS_SIZE);
    TEST_ASSERT_EQUAL(1, obsList[0].takenPosition_dev);
}

// addNewUri()
TEST(ot_app_coap_uri_obs, GivenNULLArg_WhenCallingAddNewUri_ThenReturnError)
{
    oacu_result_t result_; 
    int8_t tabDevId_ = 0;

    result_ = oac_uri_obs_addNewUri(NULL, tabDevId_, test_obs_token_4Byte, TEST_OBS_URI_INDEX_2);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenNullTokenArg_WhenCallingAddNewUri_ThenReturnError)
{
    oacu_result_t result_; 
    int8_t tabDevId_ = 0;

    result_ = oac_uri_obs_addNewUri(TEST_OBS_HANDLE, tabDevId_, NULL, TEST_OBS_URI_INDEX_2);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArgAndNoSpace_WhenCallingAddNewUri_ThenReturn_LIST_FULL)
{
    oacu_result_t result_; 
    int8_t tabDevId_ = 0;

    test_obs_fill_spaceUriTake();
    result_ = oac_uri_obs_addNewUri(TEST_OBS_HANDLE, tabDevId_, test_obs_token_4Byte, TEST_OBS_URI_INDEX_2);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_LIST_FULL, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabDevId_WhenCallingAddNewUri_ThenReturnError)
{
    oacu_result_t result_; 

    result_ = oac_uri_obs_addNewUri(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW, test_obs_token_4Byte, TEST_OBS_URI_INDEX_2);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenCallingAddNewUri_ThenReturnOK)
{
    oacu_result_t result_; 
    int8_t tabDevId_ = 0;
    int8_t tabUriId_ = 0;

    oac_uri_observer_t *obsList = oac_uri_obs_getSubListHandle();
    result_ = oac_uri_obs_addNewUri(TEST_OBS_HANDLE, tabDevId_, test_obs_token_4Byte, TEST_OBS_URI_INDEX_2);

    TEST_ASSERT_NOT_EQUAL(OAC_URI_OBS_ERROR, result_);
    TEST_ASSERT_EQUAL(tabUriId_, result_);
    TEST_ASSERT_EQUAL(TEST_OBS_URI_INDEX_2, obsList[0].uri[0].uriIndex);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_token_4Byte, obsList[0].uri[0].token, OAC_URI_OBS_TOKEN_LENGTH);
    TEST_ASSERT_EQUAL(1, obsList[0].uri[0].takenPosition_uri);
}

// tokenIsSame()
TEST(ot_app_coap_uri_obs, GivenNullHandleArg_WhenCallingTokenIsSame_ThenReturnError)
{
    oacu_result_t result_;
            
    result_ = oac_uri_obs_tokenIsSame(NULL, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_0, test_obs_token_4Byte);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenNullTokenToCheckArg_WhenCallingTokenIsSame_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_tokenIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_0, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabDevIdArg_WhenCallingTokenIsSame_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_tokenIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW, TEST_OBS_URI_INDEX_INDEX_0, test_obs_token_4Byte);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabUriIdArg_WhenCallingTokenIsSame_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_tokenIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_OVERFLOW, test_obs_token_4Byte);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenSameTokenToCheck_WhenCallingTokenIsSame_ThenReturnIs)
{
    oacu_result_t result_;
    test_obs_fillListExampleData(TEST_OBS_HANDLE);
    result_ = oac_uri_obs_tokenIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_0, test_obs_token_4Byte);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_IS, result_);
}

TEST(ot_app_coap_uri_obs, GivenDifferentTokenToCheck_WhenCallingTokenIsSame_ThenReturnIsNot)
{
    oacu_result_t result_;
    test_obs_fillListExampleData(TEST_OBS_HANDLE);
    result_ = oac_uri_obs_tokenIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, TEST_OBS_URI_INDEX_INDEX_0, test_obs_token_3Byte);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_IS_NOT, result_);
}

// tokenIsExist()
TEST(ot_app_coap_uri_obs, GivenNullHandleArg_WhenCallingTokenIsExist_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_tokenIsExist(NULL, TEST_OBS_LIST_INDEX_0, test_obs_token_4Byte_2);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenNullToken_WhenCallingTokenIsExist_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_tokenIsExist(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenNotExistToken_WhenCallingTokenIsExist_ThenReturnIsNot)
{
    oacu_result_t result_;
    test_obs_fillListExampleData(TEST_OBS_HANDLE);
    result_ = oac_uri_obs_tokenIsExist(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, test_obs_token_4Byte_2);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_IS_NOT, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabDevIdArg_WhenCallingTokenIsExist_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_tokenIsExist(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW, test_obs_token_4Byte_2);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenExistToken_WhenCallingTokenIsExist_ThenReturnIndex0)
{
    oacu_result_t result_;
    test_obs_fillListExampleData(TEST_OBS_HANDLE);
    result_ = oac_uri_obs_tokenIsExist(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, test_obs_token_4Byte);
    TEST_ASSERT_EQUAL(TEST_OBS_LIST_INDEX_0, result_);
}

// ipAddrIsSame()
TEST(ot_app_coap_uri_obs, GivenNULLArgs_WhenCallingIpAddrIsSame_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_ipAddrIsSame(NULL, TEST_OBS_LIST_INDEX_0, &ipAddr_different);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);

    result_ = oac_uri_obs_ipAddrIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverFlowTabDevIdArgs_WhenCallingIpAddrIsSame_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_ipAddrIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW, &ipAddr_different);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenSameIpdArgs_WhenCallingIpAddrIsSame_ThenReturn_IS)
{
    oacu_result_t result_;

    test_obs_fillListExampleData(TEST_OBS_HANDLE);
    result_ = oac_uri_obs_ipAddrIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, &test_obs_obsTrue.ipAddr);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_IS, result_);
}

TEST(ot_app_coap_uri_obs, GivenDifferentIpdArgs_WhenCallingIpAddrIsSame_ThenReturn_IS_NOT)
{
    oacu_result_t result_;
    
    test_obs_fillListExampleData(TEST_OBS_HANDLE);
    result_ = oac_uri_obs_ipAddrIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, &ipAddr_different);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_IS_NOT, result_);
}

// devNameFullIsSame()
TEST(ot_app_coap_uri_obs, GivenNULLArgs_WhenCallingDevNameFullIsSame_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_devNameFullIsSame(NULL, TEST_OBS_LIST_INDEX_0, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);

    result_ = oac_uri_obs_devNameFullIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenOverflowArgs_WhenCallingDevNameFullIsSame_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_devNameFullIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_OVERFLOW, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenToLongDevNameArgs_WhenCallingDevNameFullIsSame_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_devNameFullIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, deviceNameFull_too_long_32);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArgs_WhenCallingDevNameFullIsSame_ThenReturn_is)
{
    oacu_result_t result_;
    test_obs_fillListExampleData(TEST_OBS_HANDLE);
    result_ = oac_uri_obs_devNameFullIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_IS, result_);
}

TEST(ot_app_coap_uri_obs, GivenDifferentArgs_WhenCallingDevNameFullIsSame_ThenReturn_is_not)
{
    oacu_result_t result_;
    test_obs_fillListExampleData(TEST_OBS_HANDLE);
    result_ = oac_uri_obs_devNameFullIsSame(TEST_OBS_HANDLE, TEST_OBS_LIST_INDEX_0, deviceNameFull_Different);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_IS_NOT, result_);
}

// devNameFullIsExist()
TEST(ot_app_coap_uri_obs, GivenNULLArgs_WhenCallingDevNameFullIsExist_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_devNameFullIsExist(NULL, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);

    result_ = oac_uri_obs_devNameFullIsExist(TEST_OBS_HANDLE, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTooLongDevNameArgs_WhenCallingDevNameFullIsExist_ThenReturnError)
{
    oacu_result_t result_;
    result_ = oac_uri_obs_devNameFullIsExist(TEST_OBS_HANDLE, deviceNameFull_too_long_32);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenExistDevNameArgs_WhenCallingDevNameFullIsExist_ThenReturn_is)
{
    oacu_result_t result_;
    test_obs_fillListExampleData(TEST_OBS_HANDLE);
    result_ = oac_uri_obs_devNameFullIsExist(TEST_OBS_HANDLE, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_NOT_EQUAL(OAC_URI_OBS_ERROR, result_);
    TEST_ASSERT_EQUAL(0, result_);
}

TEST(ot_app_coap_uri_obs, GivenNotExistDevNameArgs_WhenCallingDevNameFullIsExist_ThenReturn_is_not)
{
    oacu_result_t result_;
    test_obs_fillListExampleData(TEST_OBS_HANDLE);
    result_ = oac_uri_obs_devNameFullIsExist(TEST_OBS_HANDLE, deviceNameFull_Different);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_IS_NOT, result_);
}


// subscribe()
TEST(ot_app_coap_uri_obs, GivenNullHandleArgS_WhenCallingSubscribe_ThenReturnError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_subscribe(NULL, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
    
    result_ = oac_uri_obs_subscribe(TEST_OBS_HANDLE, NULL, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);

    result_ = oac_uri_obs_subscribe(TEST_OBS_HANDLE, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, NULL, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);

    result_ = oac_uri_obs_subscribe(TEST_OBS_HANDLE, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenBadUriIndexArg_WhenCallingSubscribe_ThenReturnError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_subscribe(TEST_OBS_HANDLE, test_obs_obsTrue.uri->token, 0, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);    
}

TEST(ot_app_coap_uri_obs, GivenTooLongDeviceName_WhenCallingSubscribe_ThenReturnError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_subscribe(TEST_OBS_HANDLE, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, deviceNameFull_too_long_32);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);    
}

TEST(ot_app_coap_uri_obs, GivenTrueFirstDevArgs_WhenCallingSubscribe_ThenReturn_ok)
{
    oacu_result_t result_;
    oac_uri_observer_t * subList = oac_uri_obs_getSubListHandle();
    result_ = oac_uri_obs_subscribe(subList, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ADDED_NEW_DEVICE, result_); 
    TEST_ASSERT_EQUAL(1, subList[0].takenPosition_dev);
    TEST_ASSERT_EQUAL(1, subList[0].uri[0].takenPosition_uri);    
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue.uri->token, subList[0].uri[0].token, OAC_URI_OBS_TOKEN_LENGTH);
    TEST_ASSERT_EQUAL(test_obs_obsTrue.uri->uriIndex, subList[0].uri[0].uriIndex);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&test_obs_obsTrue.ipAddr, &subList[0].ipAddr, OT_IP6_ADDRESS_SIZE);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue.deviceNameFull, subList[0].deviceNameFull, strlen(test_obs_obsTrue.deviceNameFull));
} 

TEST(ot_app_coap_uri_obs, GivenTrueTwiceFirstDevArgs_WhenCallingSubscribe_ThenReturn_ok)
{
    oacu_result_t result_;
    oac_uri_observer_t * subList = oac_uri_obs_getSubListHandle();
    oac_uri_obs_subscribe(subList, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    result_ = oac_uri_obs_subscribe(subList, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_NO_NEED_UPDATE, result_); 
} 

TEST(ot_app_coap_uri_obs, GivenTrueTwiceDifferentDevArgs_WhenCallingSubscribe_ThenReturn_ok)
{
    oacu_result_t result_;
    oac_uri_observer_t * subList = oac_uri_obs_getSubListHandle();

    result_ = oac_uri_obs_subscribe(subList, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);    
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ADDED_NEW_DEVICE, result_); 
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue.uri->token, subList[0].uri[0].token, OAC_URI_OBS_TOKEN_LENGTH);
    TEST_ASSERT_EQUAL(test_obs_obsTrue.uri->uriIndex, subList[0].uri[0].uriIndex);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&test_obs_obsTrue.ipAddr, &subList[0].ipAddr, OT_IP6_ADDRESS_SIZE);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue.deviceNameFull, subList[0].deviceNameFull, strlen(test_obs_obsTrue.deviceNameFull));

    result_ = oac_uri_obs_subscribe(subList, test_obs_obsTrue2.uri->token, test_obs_obsTrue2.uri->uriIndex, &test_obs_obsTrue2.ipAddr, test_obs_obsTrue2.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ADDED_NEW_DEVICE, result_); 
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue2.uri->token, subList[1].uri[0].token, OAC_URI_OBS_TOKEN_LENGTH);
    TEST_ASSERT_EQUAL(test_obs_obsTrue2.uri->uriIndex, subList[1].uri[0].uriIndex);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&test_obs_obsTrue2.ipAddr, &subList[1].ipAddr, OT_IP6_ADDRESS_SIZE);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue2.deviceNameFull, subList[1].deviceNameFull, strlen(test_obs_obsTrue2.deviceNameFull));
} 

TEST(ot_app_coap_uri_obs, GivenTrueTwiceDeviceNameButDifferentToken_WhenCallingSubscribe_ThenReturn_ok)
{
    oacu_result_t result_;
    oac_uri_observer_t * subList = oac_uri_obs_getSubListHandle();

    result_ = oac_uri_obs_subscribe(subList, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ADDED_NEW_DEVICE, result_); 
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue.uri->token, subList[0].uri[0].token, OAC_URI_OBS_TOKEN_LENGTH);

    result_ = oac_uri_obs_subscribe(subList, test_obs_obsTrue2.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(2, result_); 
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue2.uri->token, subList[0].uri[0].token, OAC_URI_OBS_TOKEN_LENGTH);
} 

TEST(ot_app_coap_uri_obs, GivenTrueTwiceDeviceNameButDifferentUri_WhenCallingSubscribe_ThenReturn_ok)
{
    oacu_result_t result_;
    oac_uri_observer_t * subList = oac_uri_obs_getSubListHandle();

    result_ = oac_uri_obs_subscribe(subList, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ADDED_NEW_DEVICE, result_); 
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue.uri->token, subList[0].uri[0].token, OAC_URI_OBS_TOKEN_LENGTH);
    TEST_ASSERT_EQUAL(test_obs_obsTrue.uri->uriIndex, subList[0].uri[0].uriIndex);

    result_ = oac_uri_obs_subscribe(subList, test_obs_obsTrue2.uri->token, test_obs_obsTrue2.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(4, result_); 
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue2.uri->token, subList[0].uri[1].token, OAC_URI_OBS_TOKEN_LENGTH);
    TEST_ASSERT_EQUAL(test_obs_obsTrue2.uri->uriIndex, subList[0].uri[1].uriIndex);
} 

TEST(ot_app_coap_uri_obs, GivenTrueTwiceDeviceNameButDifferentIpAddr_WhenCallingSubscribe_ThenReturn_ok)
{
    oacu_result_t result_;
    oac_uri_observer_t * subList = oac_uri_obs_getSubListHandle();

    result_ = oac_uri_obs_subscribe(subList, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ADDED_NEW_DEVICE, result_); 
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&test_obs_obsTrue.ipAddr, &subList[0].ipAddr, OT_IP6_ADDRESS_SIZE);

    result_ = oac_uri_obs_subscribe(subList, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue2.ipAddr, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(1, result_); 
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&test_obs_obsTrue2.ipAddr, &subList[0].ipAddr, OT_IP6_ADDRESS_SIZE);
} 

TEST(ot_app_coap_uri_obs, GivenTrueTwiceDeviceNameButDifferentTokenAndIpAddr_WhenCallingSubscribe_ThenReturn_ok)
{
    oacu_result_t result_;
    oac_uri_observer_t * subList = oac_uri_obs_getSubListHandle();

    result_ = oac_uri_obs_subscribe(subList, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ADDED_NEW_DEVICE, result_); 
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue.uri->token, subList[0].uri[0].token, OAC_URI_OBS_TOKEN_LENGTH);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&test_obs_obsTrue.ipAddr, &subList[0].ipAddr, OT_IP6_ADDRESS_SIZE);

    result_ = oac_uri_obs_subscribe(subList, test_obs_obsTrue2.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue2.ipAddr, test_obs_obsTrue.deviceNameFull);
    TEST_ASSERT_EQUAL(3, result_); 
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue2.uri->token, subList[0].uri[0].token, OAC_URI_OBS_TOKEN_LENGTH);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&test_obs_obsTrue2.ipAddr, &subList[0].ipAddr, OT_IP6_ADDRESS_SIZE);
}


// unsubscribe()
TEST(ot_app_coap_uri_obs, GivenNullArgs_WhenCallingUnubscribe_ThenReturnError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_unsubscribe(NULL, test_obs_obsTrue.deviceNameFull, test_obs_obsTrue.uri->token);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);

    result_ = oac_uri_obs_unsubscribe(TEST_OBS_HANDLE, NULL, test_obs_obsTrue.uri->token);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);

    result_ = oac_uri_obs_unsubscribe(TEST_OBS_HANDLE, test_obs_obsTrue.deviceNameFull, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenNotExistDevName_WhenCallingUnubscribe_ThenReturnError)
{
    oacu_result_t result_;

    result_ = oac_uri_obs_unsubscribe(TEST_OBS_HANDLE, test_obs_obsTrue.deviceNameFull, test_obs_obsTrue.uri->token);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenNotExistToken_WhenCallingUnubscribe_ThenReturnError)
{
    oacu_result_t result_;

    oac_uri_obs_subscribe(TEST_OBS_HANDLE, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);

    result_ = oac_uri_obs_unsubscribe(TEST_OBS_HANDLE, test_obs_obsTrue.deviceNameFull, test_obs_token_4Byte_2);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_TOKEN_NOT_EXIST, result_);
}

TEST(ot_app_coap_uri_obs, GivenExistToken_WhenCallingUnubscribe_ThenReturn_ok)
{
    oacu_result_t result_;
    oac_uri_observer_t *subList = oac_uri_obs_getSubListHandle();

    oac_uri_obs_subscribe(subList, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue.uri->token, subList[0].uri[0].token, OAC_URI_OBS_TOKEN_LENGTH);
    TEST_ASSERT_EQUAL(test_obs_obsTrue.uri->uriIndex, subList[0].uri[0].uriIndex);
    TEST_ASSERT_EQUAL(1, subList[0].uri[0].takenPosition_uri);
    TEST_ASSERT_EQUAL(1, subList[0].takenPosition_dev);

    result_ = oac_uri_obs_unsubscribe(subList, test_obs_obsTrue.deviceNameFull, test_obs_obsTrue.uri->token);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_OK, result_);
    TEST_ASSERT_EQUAL(0, subList[0].takenPosition_dev);

    for (uint8_t i = 0; i < OAC_URI_OBS_DEVICENAME_FULL_SIZE; i++)
    {
       TEST_ASSERT_EQUAL(0, subList[0].deviceNameFull[i]);
    }

    for (uint8_t i = 0; i < OT_IP6_ADDRESS_SIZE; i++)
    {
       TEST_ASSERT_EQUAL(0, subList[0].ipAddr.mFields.m8[i]);
    }

    TEST_ASSERT_EQUAL(0, subList[0].uri[0].uriIndex);
    TEST_ASSERT_EQUAL(0, subList[0].uri[0].takenPosition_uri);    

    for (uint8_t i = 0; i < OAC_URI_OBS_TOKEN_LENGTH; i++)
    {
       TEST_ASSERT_EQUAL(0, subList[0].uri[0].token[i]);
    }        
}

TEST(ot_app_coap_uri_obs, GivenExistTokenAndTwoDifferentUris_WhenCallingUnubscribe_ThenReturn_ok)
{
    oacu_result_t result_;
    oac_uri_observer_t *subList = oac_uri_obs_getSubListHandle();

    oac_uri_obs_subscribe(subList, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    oac_uri_obs_subscribe(subList, test_obs_obsTrue2.uri->token, test_obs_obsTrue2.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);

    result_ = oac_uri_obs_unsubscribe(subList, test_obs_obsTrue.deviceNameFull, test_obs_obsTrue.uri->token);
    // not deleted
    TEST_ASSERT_EQUAL(OAC_URI_OBS_OK, result_);
    TEST_ASSERT_EQUAL(1, subList[0].takenPosition_dev);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&test_obs_obsTrue.ipAddr, &subList[0].ipAddr, OT_IP6_ADDRESS_SIZE);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&test_obs_obsTrue.deviceNameFull, &subList[0].deviceNameFull, OAC_URI_OBS_DEVICENAME_FULL_SIZE);
        
    TEST_ASSERT_EQUAL(1, subList[0].uri[1].takenPosition_uri);  
    TEST_ASSERT_EQUAL(test_obs_obsTrue2.uri->uriIndex, subList[0].uri[1].uriIndex);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_obs_obsTrue2.uri->token, subList[0].uri[1].token, OAC_URI_OBS_TOKEN_LENGTH);

    // deleted
    TEST_ASSERT_EQUAL(0, subList[0].uri[0].uriIndex);
    TEST_ASSERT_EQUAL(0, subList[0].uri[0].takenPosition_uri);    

    for (uint8_t i = 0; i < OAC_URI_OBS_TOKEN_LENGTH; i++)
    {
       TEST_ASSERT_EQUAL(0, subList[0].uri[0].token[i]);
    }        
}

// notify()
TEST(ot_app_coap_uri_obs, GivenNullHandleArg_WhenCallingNotify_ThenReturnError)
{
    oacu_result_t result_;
    uint8_t data_ = 255;

    result_ = oac_uri_obs_notify(NULL, TEST_OBS_URI_INDEX_2, &data_, 1);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);

    result_ = oac_uri_obs_notify(TEST_OBS_HANDLE, TEST_OBS_URI_INDEX_2, NULL, 1);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);

    result_ = oac_uri_obs_notify(TEST_OBS_HANDLE, TEST_OBS_URI_INDEX_2, &data_, OAC_URI_OBS_BUFFER_SIZE+1);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenIncorrectUriArg_WhenCallingNotify_ThenReturnError)
{
    oacu_result_t result_;
    uint8_t data_ = 255;

    result_ = oac_uri_obs_notify(TEST_OBS_HANDLE, 0, &data_, 1);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}

TEST(ot_app_coap_uri_obs, GivenTrueArgs_WhenEmptySubscribeListCallingNotify_ThenReturnOk)
{
    oacu_result_t result_;
    uint8_t data_ = 255;

    result_ = oac_uri_obs_notify(TEST_OBS_HANDLE, TEST_OBS_URI_INDEX_2, &data_, 1);
    TEST_ASSERT_EQUAL(0, result_);   
}

TEST(ot_app_coap_uri_obs, GivenTrueArgs_WhenSubscribeListIsFullCallingNotify_ThenReturnOk)
{
    oacu_result_t result_;
    uint8_t data_[OAC_URI_OBS_BUFFER_SIZE];
    oac_uri_observer_t *subList = oac_uri_obs_getSubListHandle();

    for (uint16_t i = 0; i < OAC_URI_OBS_BUFFER_SIZE ; i++)
    {
        data_[i] = i;
    }
    
    test_obs_fillListExampleData(subList);
    test_obs_fillTxBuffer(subList, data_, OAC_URI_OBS_BUFFER_SIZE);

    result_ = oac_uri_obs_notify(subList, TEST_OBS_URI_INDEX_2, data_, OAC_URI_OBS_BUFFER_SIZE);
    TEST_ASSERT_EQUAL(20, result_);

    TEST_ASSERT_EQUAL(20, otapp_coapSendPutUri_subscribed_uris_fake.call_count);                                                // check count of calls
    TEST_ASSERT_EQUAL_UINT8_ARRAY(&subList[0].ipAddr, otapp_coapSendPutUri_subscribed_uris_fake.arg0_val, OT_IP6_ADDRESS_SIZE); // check ipAddr
    TEST_ASSERT_EQUAL_UINT8_ARRAY(txRxBuffer, otapp_coapSendPutUri_subscribed_uris_fake.arg1_val, otapp_coapSendPutUri_subscribed_uris_fake.arg2_val); // check created buffer
   
}

TEST(ot_app_coap_uri_obs, CheckNotify_GivenTrueArgs_WhenCallingNotify_ThenReturnOk)
{
    oacu_result_t result_;
    uint8_t data_ = 254;
    uint8_t dataSize_ = 1;

    const otIp6Address *ipAddrFromNotify;
    const uint8_t *dataFromNotify;

    oac_uri_obs_subscribe(TEST_OBS_HANDLE, test_obs_obsTrue.uri->token, test_obs_obsTrue.uri->uriIndex, &test_obs_obsTrue.ipAddr, test_obs_obsTrue.deviceNameFull);
    result_ = oac_uri_obs_notify(TEST_OBS_HANDLE, test_obs_obsTrue.uri->uriIndex, &data_, dataSize_);

    ipAddrFromNotify = otapp_coapSendPutUri_subscribed_uris_fake.arg0_val;
    dataFromNotify = otapp_coapSendPutUri_subscribed_uris_fake.arg1_val;

    oac_uri_obs_parseMessageFromNotify(dataFromNotify, &test_obs_dataPacketOut);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(&test_obs_obsTrue.ipAddr, ipAddrFromNotify, OT_IP6_ADDRESS_SIZE);
    
    TEST_ASSERT_EQUAL(data_, test_obs_dataPacketOut.buffer[0]);
    TEST_ASSERT_EQUAL(1, result_); // one subscriber

}

// parseMessage()
TEST(ot_app_coap_uri_obs, GivenNullArgs_WhenParseMessage_ThenReturnError)
{
    oacu_result_t result_;
    oac_uri_dataPacket_t *dataPacketIn;
    uint8_t data_ = 255; 
    uint8_t dataSize_ = 1; 

    dataPacketIn = test_obs_createDataPacket(test_obs_token_4Byte, &data_, dataSize_);

    result_ = oac_uri_obs_parseMessageFromNotify(NULL, &test_obs_dataPacketOut);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);

    result_ = oac_uri_obs_parseMessageFromNotify((uint8_t*)dataPacketIn, NULL);
    TEST_ASSERT_EQUAL(OAC_URI_OBS_ERROR, result_);
}


TEST(ot_app_coap_uri_obs, GivenTrueArg_WhenParseMessage_ThenReturnPtrToStract)
{
    oac_uri_dataPacket_t *dataPacketIn;
    uint8_t data_ = 255; 
    uint8_t dataSize_ = 1; 

    dataPacketIn = test_obs_createDataPacket(test_obs_token_4Byte, &data_, dataSize_);
    oac_uri_obs_parseMessageFromNotify((uint8_t*)dataPacketIn,  &test_obs_dataPacketOut);

     TEST_ASSERT_EQUAL_UINT8_ARRAY(dataPacketIn, &test_obs_dataPacketOut, sizeof(oac_uri_dataPacket_t));
}

// notify()
