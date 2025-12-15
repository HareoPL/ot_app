#include "unity_fixture.h"
#include "ot_app_pair.h"

#define UT_OAP_DEVICE_INDEX_0 (0)
#define UT_OAP_DEVICE_INDEX_1 (1)
#define UT_OAP_DEVICE_INDEX_BAD (OTAPP_PAIR_DEVICES_MAX)

#define UT_OAP_URI_INDEX_BAD    (OTAPP_PAIR_URI_MAX)
#define UT_OAP_URI_INDEX_OK     (OTAPP_URI_WELL_KNOWN_CORE)
#define UT_OAP_URI_INDEX_0      (0)
#define UT_OAP_URI_INDEX_1      (1)
#define UT_OAP_URI_INDEX_MAX    (OTAPP_PAIR_URI_MAX - 1)

#define UT_OAP_URI_TABLE_1      (OTAPP_URI_WELL_KNOWN_CORE)
#define UT_OAP_URI_TABLE_2      (OTAPP_URI_TEST)
#define UT_OAP_URI_TABLE_20     (OTAPP_URI_MOCK_20)

#define EXPECTED_URI_RETURN(devIndex, uriIndex) ((devIndex << 8) | (uriIndex)) 

static char *deviceNameFull_0 = {"device1_1_588c81fffe301ea1"};
static char *deviceNameFull_1 = {"device1_1_588c81fffe301ea2"};
static char *deviceNameFull_2 = {"device1_1_588c81fffe301ea3"};
static char *deviceNameFull_3 = {"device1_1_588c81fffe301ea4"};
static char *deviceNameFull_4 = {"device1_1_588c81fffe301ea5"};
static char *deviceNameFull_5 = {"device1_1_588c81fffe301ea6"};
static char *deviceNameFull_6 = {"device1_1_588c81fffe301ea7"};
static char *deviceNameFull_7 = {"device1_1_588c81fffe301ea8"};
static char *deviceNameFull_8 = {"device1_1_588c81fffe301ea9"};
static char *deviceNameFull_9 = {"device1_1_588c81fffe301e10"};

static otIp6Address ipAddr = {
    .mFields.m8 = {0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
                   0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x34}
};

static otapp_coap_uri_t coap_uri[] = {   
    {OTAPP_LIGHTING_ON_OFF,  {"light/on_off", NULL, NULL, NULL},},
    {OTAPP_LIGHTING_DIMM,    {"light/dimm", NULL, NULL, NULL},},
    {OTAPP_THERMOSTAT,          {"termo", NULL, NULL, NULL},},
    {OTAPP_LIGHTING_RGB,        {"light_rgb", NULL, NULL, NULL},},
    {OTAPP_ENVIRONMENT_SENSOR,  {"sensor", NULL, NULL, NULL},},
 
};
#define UT_OAP_URI_SIZE (sizeof(coap_uri) / sizeof(coap_uri[0]))

otapp_pair_resUrisBuffer_t resUrisBuffer_filled[OTAPP_PAIR_URI_MAX];
otapp_pair_resUrisBuffer_t *fillResUrisBuffer()
{
    uint8_t *buf = (uint8_t*)resUrisBuffer_filled;

    for (uint8_t i = 0; i < OTAPP_PAIR_URI_MAX; i++)
    {
        strcpy((char*)buf, coap_uri[i].resource.mUriPath);

        buf += OTAPP_URI_MAX_NAME_LENGHT;
        *buf = coap_uri[i].devType;

        buf += sizeof(otapp_deviceType_t);
        *buf = 1;

        buf += sizeof(uint8_t);
    }
    
   
    
    return resUrisBuffer_filled;
}


static int8_t ut_oap_deviceAddFullFill(void)
{
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_0, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_1, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_2, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_3, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_4, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_5, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_6, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_7, &ipAddr);
    otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_8, &ipAddr);
    return otapp_pair_DeviceAdd(otapp_pair_getHandle(), deviceNameFull_9, &ipAddr);
}

otapp_pair_resUrisParseData_t ut_oap_uriData_1 = {
    .devTypeUriFn = 2,
    .obs = 1,
    .uri = "light/on_off",
};
oacu_token_t ut_oap_token_1[4] = {0xA1, 0xB2, 0xC3, 0xD4};


TEST_GROUP(ot_app_pair_UriIndex);

TEST_SETUP(ot_app_pair_UriIndex)
{
    otapp_pair_DeviceDeleteAll(otapp_pair_getHandle());
    
    ut_oap_deviceAddFullFill();
}

TEST_TEAR_DOWN(ot_app_pair_UriIndex)
{
    /* Cleanup after every test */
}
// otapp_pair_uriResourcesCreate
TEST(ot_app_pair_UriIndex, GivenNullArg_WhenCallingUriResourcesCreate_ThenReturnError)
{
    int8_t result;
    uint16_t bufferSize = 0; 
    otapp_pair_resUrisBuffer_t *buffer;

    buffer = otapp_pair_uriResourcesCreate(NULL, UT_OAP_URI_SIZE, &result, &bufferSize);
    TEST_ASSERT_EQUAL(NULL, buffer);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair_UriIndex, GivenZeroUriSize_WhenCallingUriResourcesCreate_ThenReturnError)
{
    int8_t result;   
    uint16_t bufferSize = 0; 
    otapp_pair_resUrisBuffer_t *buffer;

    buffer = otapp_pair_uriResourcesCreate(coap_uri, 0, &result, &bufferSize);
    TEST_ASSERT_EQUAL(NULL, buffer);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);    
}

TEST(ot_app_pair_UriIndex, GivenNullBufferSize_WhenCallingUriResourcesCreate_ThenReturnError)
{
    int8_t result;   
    otapp_pair_resUrisBuffer_t *buffer;

    buffer = otapp_pair_uriResourcesCreate(coap_uri, UT_OAP_URI_SIZE, &result, NULL);
    TEST_ASSERT_EQUAL(NULL, buffer);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);    
}

TEST(ot_app_pair_UriIndex, GivenTruArgsSize1_WhenCallingUriResourcesCreate_ThenReturnOK)
{
    int8_t result;   
    uint8_t uriSize = 1; 
    uint16_t bufferSize = 0;
    uint16_t bufferSize_expected = (OTAPP_PAIR_URI_RESOURCE_BUFFER_SIZE * uriSize); 
    otapp_pair_resUrisBuffer_t *buffer;
    otapp_pair_resUrisBuffer_t *bufferFilled = fillResUrisBuffer();

    buffer = otapp_pair_uriResourcesCreate(coap_uri, uriSize, &result, &bufferSize);
    TEST_ASSERT_NOT_EQUAL(NULL, buffer);
    TEST_ASSERT_EQUAL(bufferSize_expected, bufferSize);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_OK, result);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bufferFilled, buffer, bufferSize);
}

TEST(ot_app_pair_UriIndex, GivenTruArgsSize3_WhenCallingUriResourcesCreate_ThenReturnOK)
{
    int8_t result;   
    uint8_t uriSize = 3; 
    uint16_t bufferSize = 0;
    uint16_t bufferSize_expected = (OTAPP_PAIR_URI_RESOURCE_BUFFER_SIZE * uriSize); 
    otapp_pair_resUrisBuffer_t *buffer;
    otapp_pair_resUrisBuffer_t *bufferFilled = fillResUrisBuffer();

    buffer = otapp_pair_uriResourcesCreate(coap_uri, uriSize, &result, &bufferSize);
    TEST_ASSERT_NOT_EQUAL(NULL, buffer);
    TEST_ASSERT_EQUAL(bufferSize_expected, bufferSize);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_OK, result);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bufferFilled, buffer, bufferSize);
}

TEST(ot_app_pair_UriIndex, GivenTruArgsSizeMax_WhenCallingUriResourcesCreate_ThenReturnOK)
{
    int8_t result;   
    uint8_t uriSize = OTAPP_PAIR_URI_MAX; 
    uint16_t bufferSize = 0;
    uint16_t bufferSize_expected = (OTAPP_PAIR_URI_RESOURCE_BUFFER_SIZE * uriSize); 
    otapp_pair_resUrisBuffer_t *buffer;
    otapp_pair_resUrisBuffer_t *bufferFilled = fillResUrisBuffer();

    buffer = otapp_pair_uriResourcesCreate(coap_uri, uriSize, &result, &bufferSize);
    TEST_ASSERT_NOT_EQUAL(NULL, buffer);
    TEST_ASSERT_EQUAL(bufferSize_expected, bufferSize);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_OK, result);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bufferFilled, buffer, bufferSize);
}

TEST(ot_app_pair_UriIndex, GivenOvervflowSize_WhenCallingUriResourcesCreate_ThenReturnError)
{
    int8_t result;   
    uint8_t uriSize = OTAPP_PAIR_URI_MAX + 1; 
    uint16_t bufferSize = 0;
    otapp_pair_resUrisBuffer_t *buffer;

    buffer = otapp_pair_uriResourcesCreate(coap_uri, uriSize, &result, &bufferSize);
    TEST_ASSERT_EQUAL(NULL, buffer);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair_UriIndex, GivenNullResultArg_WhenCallingUriResourcesCreate_ThenReturnError)
{
    uint8_t uriSize = OTAPP_PAIR_URI_MAX + 1; 
    uint16_t bufferSize = 0;
    otapp_pair_resUrisBuffer_t *buffer;

    buffer = otapp_pair_uriResourcesCreate(coap_uri, uriSize, NULL, &bufferSize);
    TEST_ASSERT_EQUAL(NULL, buffer);
}

// otapp_pair_uriParseMessage
TEST(ot_app_pair_UriIndex, GivenNullInBuffer_WhenCallinguriParseMessage_ThenReturnError)
{
    int8_t result;
    otapp_pair_resUrisParseData_t *parseData;
    uint16_t parsedDataSize = 0;
    
    parseData = otapp_pair_uriParseMessage(NULL, 1, &result, &parsedDataSize);
    TEST_ASSERT_EQUAL(NULL, parseData);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair_UriIndex, GivenZeroBufSize_WhenCallinguriParseMessage_ThenReturnError)
{
    int8_t result;
    otapp_pair_resUrisParseData_t *parseData;
    uint16_t parsedDataSize = 0;
    otapp_pair_resUrisBuffer_t *bufferFilled = fillResUrisBuffer();

    parseData = otapp_pair_uriParseMessage((uint8_t*)bufferFilled, 0, &result, &parsedDataSize);
    TEST_ASSERT_EQUAL(NULL, parseData);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair_UriIndex, GivenNullOutParsedDataSize_WhenCallinguriParseMessage_ThenReturnError)
{
    int8_t result;
    otapp_pair_resUrisParseData_t *parseData;
    otapp_pair_resUrisBuffer_t *bufferFilled = fillResUrisBuffer();

    parseData = otapp_pair_uriParseMessage((uint8_t*)bufferFilled, 1, &result, NULL);
    TEST_ASSERT_EQUAL(NULL, parseData);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}


TEST(ot_app_pair_UriIndex, GivenTrueArgsSize1_WhenCallinguriParseMessage_ThenReturnOK)
{
    int8_t result;   
    uint16_t uriSize = 1 * OTAPP_PAIR_URI_RESOURCE_BUFFER_SIZE; 
    otapp_pair_resUrisParseData_t *parseData;
    uint16_t parsedDataSize = 0;
    uint16_t parsedDataSize_expected = (uriSize / sizeof(otapp_pair_resUrisParseData_t));
    otapp_pair_resUrisBuffer_t *bufferFilled = fillResUrisBuffer();

    parseData = otapp_pair_uriParseMessage((uint8_t*)bufferFilled, uriSize, &result, &parsedDataSize);
    TEST_ASSERT_NOT_EQUAL(NULL, parseData);
    TEST_ASSERT_EQUAL(parsedDataSize_expected, parsedDataSize);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_OK, result);
    TEST_ASSERT_EQUAL_UINT8_ARRAY((uint8_t*)bufferFilled, (uint8_t*)parseData, uriSize);
}

TEST(ot_app_pair_UriIndex, GivenTrueArgsSize3_WhenCallinguriParseMessage_ThenReturnOK)
{
    int8_t result;   
    uint8_t uriSize = 3 * OTAPP_PAIR_URI_RESOURCE_BUFFER_SIZE; 
    otapp_pair_resUrisParseData_t *parseData;
    uint16_t parsedDataSize = 0;
    uint16_t parsedDataSize_expected = (uriSize / sizeof(otapp_pair_resUrisParseData_t));
    otapp_pair_resUrisBuffer_t *bufferFilled = fillResUrisBuffer();

    parseData = otapp_pair_uriParseMessage((uint8_t*)bufferFilled, uriSize, &result, &parsedDataSize);
    TEST_ASSERT_NOT_EQUAL(NULL, parseData);
    TEST_ASSERT_EQUAL(parsedDataSize_expected, parsedDataSize);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_OK, result);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bufferFilled, parseData, uriSize);
}

TEST(ot_app_pair_UriIndex, GivenTrueArgsSizeMax_WhenCallinguriParseMessage_ThenReturnOK)
{
    int8_t result;   
    uint8_t uriSize = OTAPP_PAIR_URI_MAX * OTAPP_PAIR_URI_RESOURCE_BUFFER_SIZE; 
    otapp_pair_resUrisParseData_t *parseData;
    uint16_t parsedDataSize = 0;
    uint16_t parsedDataSize_expected = (uriSize / sizeof(otapp_pair_resUrisParseData_t));
    otapp_pair_resUrisBuffer_t *bufferFilled = fillResUrisBuffer();

    parseData = otapp_pair_uriParseMessage((uint8_t*)bufferFilled, uriSize, &result, &parsedDataSize);
    TEST_ASSERT_NOT_EQUAL(NULL, parseData);
    TEST_ASSERT_EQUAL(parsedDataSize_expected, parsedDataSize);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_OK, result);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(bufferFilled, parseData, uriSize);
}

TEST(ot_app_pair_UriIndex, GivenOverflowSize_WhenCallinguriParseMessage_ThenReturnError)
{
    int8_t result;   
    uint8_t uriSize = (OTAPP_PAIR_URI_MAX + 1) * OTAPP_PAIR_URI_RESOURCE_BUFFER_SIZE; 
    otapp_pair_resUrisParseData_t *parseData;
    uint16_t parsedDataSize = 0;
    otapp_pair_resUrisBuffer_t *bufferFilled = fillResUrisBuffer();

    parseData = otapp_pair_uriParseMessage((uint8_t*)bufferFilled, uriSize, &result, &parsedDataSize);
    TEST_ASSERT_EQUAL(NULL, parseData);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

//otapp_pair_uriAdd
TEST(ot_app_pair_UriIndex, GivenNullDeviceUrisList_WhenCallingUriAdd_ThenReturnError)
{
    int8_t result;
   
    result = otapp_pair_uriAdd(NULL, &ut_oap_uriData_1, ut_oap_token_1);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair_UriIndex, GivenNulluriData_WhenCallingUriAdd_ThenReturnError)
{
    int8_t result, devId;
    otapp_pair_DeviceList_t *deviceListHandle;
    otapp_pair_Device_t *newDevice;

    deviceListHandle = otapp_pair_getHandle();
    newDevice = otapp_pair_DeviceGet(deviceListHandle, deviceNameFull_7);

    result = otapp_pair_uriAdd(&newDevice->urisList[0], NULL, ut_oap_token_1);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair_UriIndex, GivenTrue_WhenCallingUriAdd_ThenReturnError_OK)
{
    int8_t result ;
    otapp_pair_DeviceList_t *deviceListHandle;
    otapp_pair_Device_t *newDevice;

    deviceListHandle = otapp_pair_getHandle();
    newDevice = otapp_pair_DeviceGet(deviceListHandle, deviceNameFull_7);

    result = otapp_pair_uriAdd(&newDevice->urisList[0], &ut_oap_uriData_1, ut_oap_token_1);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_OK, result);
}

// otapp_pair_subSendUpdateIP
TEST(ot_app_pair_UriIndex, GivenNull_WhenCallingSubUpdateIP_ThenReturnError)
{
    int8_t result;
    
    result = otapp_pair_subSendUpdateIP(NULL);
    TEST_ASSERT_EQUAL(OTAPP_PAIR_ERROR, result);
}

TEST(ot_app_pair_UriIndex, GivenTrueArgEmptyDeviceList_WhenCallingSubUpdateIP_ThenReturn_0)
{
    int8_t result;
    
    result = otapp_pair_subSendUpdateIP(otapp_pair_getHandle());
    TEST_ASSERT_EQUAL(0, result);
}

TEST(ot_app_pair_UriIndex, GivenTrueArgFillDeviceList_WhenCallingSubUpdateIP_ThenReturn_1)
{
    int8_t result;
    otapp_pair_Device_t *newDevice;

    newDevice = otapp_pair_DeviceGet(otapp_pair_getHandle(), deviceNameFull_7);
    otapp_pair_uriAdd(&newDevice->urisList[0], &ut_oap_uriData_1, ut_oap_token_1);
    
    result = otapp_pair_subSendUpdateIP(otapp_pair_getHandle());
    TEST_ASSERT_EQUAL(1, result);
}