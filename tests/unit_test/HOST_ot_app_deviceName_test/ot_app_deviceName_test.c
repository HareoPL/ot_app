#include "unity_fixture.h"
#include "ot_app_deviceName.h"
#include "string.h"

#define UT_DN_OK_DEVICE_TYPE_1                  OTAPP_CONTROL_PANEL
#define UT_DN_OK_DEVICE_TYPE_2_SWITCH           OTAPP_SWITCH
#define UT_DN_OK_DEVICE_TYPE_3_LIGHT            OTAPP_LIGHTING_ON_OFF

#define UT_DN_BAD_DEVICE_TYPE_0                 OTAPP_NO_DEVICE_TYPE
#define UT_DN_BAD_DEVICE_TYPE_OUTSIDE_SCOPE     OTAPP_END_OF_DEVICE_TYPE

static char *deviceName_device1 = {"device1"};
static char *deviceName_device2 = {"device2"};
static char *deviceName_max_10_byte = {"dMax10byte"};
static char *deviceName_to_long = {"name11_byte"};

static char *deviceNameFull_to_long = {"device1_1_588c81fffe301ea4_too_long"};
static char *deviceNameFull_not_same = {"device2_1_588c81fffe301ea4"};
static char *deviceNameFull_device1_type0_fakeAddr = {"device1_0_0011223344556677"};
static char *deviceNameFull_device1_type0_fakeAddr_too_long = {"device1_0_00112233445566779900"};
static char *deviceNameFull_device1_type0_fakeAddr_too_short = {"device1_0_00112233445566"};
static char *deviceNameFull_device1_type0_fakeAddr_eui = {"0011223344556677"};
static char *deviceNameFull_device1_type1_fakeAddr = {"device1_1_0011223344556677"};

static const char *ut_dn_domain = ".default.service.arpa.";
static char *deviceName_with_domain_OK = {"device1_1_0011223344556677.default.service.arpa."};

static char ut_dn_charBuf[OTAPP_DEVICENAME_MIN_ADD_DOMAIN_BUFFER_SIZE];
char *ut_dn_createDeviceNameFull(const char *deviceName, const uint8_t deviceType) 
{
    snprintf(ut_dn_charBuf, OTAPP_DNS_SRV_LABEL_SIZE - 1, "%s_%d_0011223344556677", deviceName, deviceType);
    
    return ut_dn_charBuf;
}
char *ut_dn_createHostName(const char *deviceName, const uint8_t deviceType)
{
    ut_dn_createDeviceNameFull(deviceName, deviceType);
    strcat(ut_dn_charBuf, ut_dn_domain);

    return ut_dn_charBuf;
}

void ut_dn_clearBuffer()
{
    memset(ut_dn_charBuf, 0, OTAPP_DEVICENAME_MIN_ADD_DOMAIN_BUFFER_SIZE);
}

TEST_GROUP(ot_app_deviceName);

TEST_SETUP(ot_app_deviceName)
{
    /* Init before every test */
    otapp_deviceNameDelete();
    ut_dn_clearBuffer();
}

TEST_TEAR_DOWN(ot_app_deviceName)
{
    /* Cleanup after every test */
}

TEST(ot_app_deviceName, GivenNULLdevName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(NULL, UT_DN_OK_DEVICE_TYPE_1);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenCorrectDevName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(deviceName_device1, UT_DN_OK_DEVICE_TYPE_1);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_OK, result);
}

TEST(ot_app_deviceName, GivenIncorrectDeviceName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(deviceName_device1, UT_DN_BAD_DEVICE_TYPE_0);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenOutsideScopeDeviceName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(deviceName_device1, UT_DN_BAD_DEVICE_TYPE_OUTSIDE_SCOPE);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenCorrectDeviceName_WhenIsCallingDeviceNameSet_ThenReturnOK)
{
    int8_t result;

    result = otapp_deviceNameSet(deviceName_device1, UT_DN_OK_DEVICE_TYPE_1);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_OK, result);
}

TEST(ot_app_deviceName, GivenTooLongDevName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(deviceName_to_long, UT_DN_OK_DEVICE_TYPE_1);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_TOO_LONG, result);
}

TEST(ot_app_deviceName, GivenMaxLengthDevName_WhenIsCallingDeviceNameSet_ThenReturnError)
{
    int8_t result;

    result = otapp_deviceNameSet(deviceName_max_10_byte, UT_DN_OK_DEVICE_TYPE_1);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_OK, result);
}

// deviceNameFullGet
TEST(ot_app_deviceName, GivenNoCallingDeviceNameSet_WhenIsCallingDeviceNameFullGet_ThenReturnError)
{
    const char *devNameFull;

    devNameFull = otapp_deviceNameFullGet();
    TEST_ASSERT_EQUAL_STRING(NULL, devNameFull);
}

TEST(ot_app_deviceName, GivenCallingDeviceNameSet_WhenIsCallingDeviceNameFullGet_ThenReturnOK)
{
    const char *devNameFull;

    otapp_deviceNameSet(deviceName_device1, UT_DN_OK_DEVICE_TYPE_1);
    devNameFull = otapp_deviceNameFullGet();
    TEST_ASSERT_EQUAL_STRING(deviceNameFull_device1_type1_fakeAddr, devNameFull);
}

// deviceNameFullIsSame
TEST(ot_app_deviceName, GivenNullArgument_WhenIsCallingdeviceNameFullIsSame_ThenReturnError)
{
    int8_t result;
    result = otapp_deviceNameFullIsSame(NULL);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenTooLongDevNameFull_WhenIsCallingdeviceNameFullIsSame_ThenReturnError)
{
    int8_t result;
    result = otapp_deviceNameFullIsSame(deviceNameFull_to_long);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_TOO_LONG, result);
}

TEST(ot_app_deviceName, GivenNotSameDevNameFull_WhenIsCallingdeviceNameFullIsSame_ThenReturnNOT)
{
    int8_t result;
    otapp_deviceNameSet(deviceName_device1, UT_DN_OK_DEVICE_TYPE_1);    
    result = otapp_deviceNameFullIsSame(deviceNameFull_not_same);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_IS_NOT, result);
}

TEST(ot_app_deviceName, GivenSameDevNameFull_WhenIsCallingdeviceNameFullIsSame_ThenReturnYES)
{
    int8_t result;
    otapp_deviceNameSet(deviceName_device1, UT_DN_OK_DEVICE_TYPE_1);    
    result = otapp_deviceNameFullIsSame(deviceNameFull_device1_type1_fakeAddr);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_IS, result);
}

// otapp_deviceNameIsSame
TEST(ot_app_deviceName, GivenNullArgument_WhenIsCallingDeviceNameIsSame_ThenReturnError)
{
    int8_t result;
    result = otapp_deviceNameIsSame(NULL, strlen(deviceName_device2));
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenNotCallingDeviceNameSet_WhenIsCallingDeviceNameIsSame_ThenReturnError)
{
    int8_t result;
    result = otapp_deviceNameIsSame(deviceNameFull_device1_type0_fakeAddr, strlen(deviceNameFull_device1_type0_fakeAddr));
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_CALL_DEVICE_NAME_SET_FN, result);
} 

TEST(ot_app_deviceName, GivenTooLongDevNameFull_WhenIsCallingDeviceNameIsSame_ThenReturnError)
{
    int8_t result;
    result = otapp_deviceNameIsSame(deviceNameFull_to_long, strlen(deviceNameFull_to_long));
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_TOO_LONG, result);
}
 
TEST(ot_app_deviceName, GivenTooShortDevNameFull_WhenIsCallingDeviceNameIsSame_ThenReturnError)
{
    int8_t result;
    otapp_deviceNameSet(deviceName_device1, UT_DN_OK_DEVICE_TYPE_1);
    result = otapp_deviceNameIsSame(deviceName_device1, strlen(deviceName_device1));
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_TOO_SHORT, result);
}   

TEST(ot_app_deviceName, GivenNotSameDevNameFull_WhenIsCallingDeviceNameIsSame_ThenReturnNOT)
{
    int8_t result;
    otapp_deviceNameSet(deviceName_device1, UT_DN_OK_DEVICE_TYPE_1);    
    result = otapp_deviceNameIsSame(deviceNameFull_not_same, strlen(deviceNameFull_not_same));
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_IS_NOT, result);
}

TEST(ot_app_deviceName, GivenSameDevNameFull_WhenIsCallingDeviceNameIsSame_ThenReturnYES)
{
    int8_t result;
    otapp_deviceNameSet(deviceName_device1, UT_DN_OK_DEVICE_TYPE_1);    
    result = otapp_deviceNameIsSame(deviceNameFull_device1_type0_fakeAddr, strlen(deviceNameFull_device1_type0_fakeAddr));
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_IS, result);
}

// otapp_deviceNameGetDevId
TEST(ot_app_deviceName, GivenNullDevNameFull_WhenIsCallingDeviceNameGetDevId_ThenReturnError)
{
    int8_t result;   
    result = otapp_deviceNameGetDevId(NULL, strlen(deviceNameFull_device1_type0_fakeAddr));
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenWrongTypeDevInDevNameFull_WhenIsCallingDeviceNameGetDevId_ThenReturnError)
{
    int8_t result;   
    result = otapp_deviceNameGetDevId(deviceNameFull_device1_type0_fakeAddr, strlen(deviceNameFull_device1_type0_fakeAddr));
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenCorrectTypeDevInDevNameFull_WhenIsCallingDeviceNameGetDevId_ThenReturnOK)
{
    int8_t result;
    char *_deviceNameFull = ut_dn_createDeviceNameFull(deviceName_device1, UT_DN_OK_DEVICE_TYPE_1);

    result = otapp_deviceNameGetDevId(_deviceNameFull, strlen(_deviceNameFull));
    TEST_ASSERT_EQUAL(UT_DN_OK_DEVICE_TYPE_1, result);
}

TEST(ot_app_deviceName, GivenOutsideScopeTypeDevInDevNameFull_WhenIsCallingDeviceNameGetDevId_ThenReturnError)
{
    int8_t result;
    char *_deviceNameFull = ut_dn_createDeviceNameFull(deviceName_device1, UT_DN_BAD_DEVICE_TYPE_OUTSIDE_SCOPE);
    
    result = otapp_deviceNameGetDevId(_deviceNameFull, strlen(_deviceNameFull));
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

// otapp_deviceNameFullAddDomain
TEST(ot_app_deviceName, GivenNullDevNameFull_WhenIsCallingDeviceNameFullAddDomain_ThenReturnError)
{
    int8_t result;   
    result = otapp_deviceNameFullAddDomain(NULL, OTAPP_DEVICENAME_MIN_ADD_DOMAIN_BUFFER_SIZE);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenTooSmallBuffer_WhenIsCallingDeviceNameFullAddDomain_ThenReturnError)
{
    int8_t result;   
    result = otapp_deviceNameFullAddDomain(ut_dn_charBuf, (OTAPP_DEVICENAME_MIN_ADD_DOMAIN_BUFFER_SIZE - 32));
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_BUFFER_TOO_SMALL, result);
}

TEST(ot_app_deviceName, GivenProperBufferSizeWithoutDevName_WhenIsCallingDeviceNameFullAddDomain_ThenReturnError)
{
    int8_t result;   
    result = otapp_deviceNameFullAddDomain(ut_dn_charBuf, OTAPP_DEVICENAME_MIN_ADD_DOMAIN_BUFFER_SIZE);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_TOO_SHORT, result);
}

TEST(ot_app_deviceName, GivenProperBufferSizeWithDevName_WhenIsCallingDeviceNameFullAddDomain_ThenReturnOK)
{
    int8_t result;
    char *_deviceNameFull = ut_dn_createDeviceNameFull(deviceName_device1, UT_DN_OK_DEVICE_TYPE_1);
    result = otapp_deviceNameFullAddDomain(_deviceNameFull, OTAPP_DEVICENAME_MIN_ADD_DOMAIN_BUFFER_SIZE);
    TEST_ASSERT_EQUAL_STRING(deviceName_with_domain_OK, _deviceNameFull);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_OK, result);
}

TEST(ot_app_deviceName, GivenTooLongDevNameFull_WhenIsCallingDeviceNameFullAddDomain_ThenReturnError)
{
    int8_t result;   
    result = otapp_deviceNameFullAddDomain(deviceNameFull_to_long, OTAPP_DEVICENAME_MIN_ADD_DOMAIN_BUFFER_SIZE);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_TOO_LONG, result);
}

TEST(ot_app_deviceName, GivenTooShortDevNameFull_WhenIsCallingDeviceNameFullAddDomain_ThenReturnError)
{
    int8_t result;   
    result = otapp_deviceNameFullAddDomain(deviceName_device1, OTAPP_DEVICENAME_MIN_ADD_DOMAIN_BUFFER_SIZE);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_TOO_SHORT, result);
}

// otapp_hostNameToDeviceNameFull
TEST(ot_app_deviceName, GivenNullHostName_WhenIsCallingHostNameToDeviceNameFull_ThenReturnError)
{
    int8_t result;   
    result = otapp_hostNameToDeviceNameFull(NULL);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenWithoutDomainHostName_WhenIsCallingHostNameToDeviceNameFull_ThenReturnError)
{
    int8_t result;   
    result = otapp_hostNameToDeviceNameFull(deviceNameFull_device1_type1_fakeAddr);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenTrueHostName_WhenIsCallingHostNameToDeviceNameFull_ThenReturnOK_PTR_STR)
{
    int8_t result;
    char *_hostName = ut_dn_createHostName(deviceName_device1, UT_DN_OK_DEVICE_TYPE_1);
    result = otapp_hostNameToDeviceNameFull(_hostName);

    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_OK , result);
    TEST_ASSERT_EQUAL_STRING(deviceNameFull_device1_type1_fakeAddr, _hostName);
}

// otapp_deviceNameIsMatching
TEST(ot_app_deviceName, GivenNullDeviceNameFull_WhenIsCallingDeviceNameIsMatching_ThenReturnError)
{
    int8_t result;   
    result = otapp_deviceNameIsMatching(NULL);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenSameDeviceNameFull_WhenIsCallingDeviceNameIsMatching_ThenReturnError)
{
    int8_t result;
    otapp_deviceNameSet(deviceName_device1, UT_DN_OK_DEVICE_TYPE_2_SWITCH);
    result = otapp_deviceNameIsMatching(otapp_deviceNameFullGet());
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_IS_NOT, result);
}

TEST(ot_app_deviceName, GivenMatchingDeviceNameFull_WhenIsCallingDeviceNameIsMatching_ThenReturnOK)
{
    int8_t result;
    char *_deviceNameFull = ut_dn_createDeviceNameFull(deviceName_device1, UT_DN_OK_DEVICE_TYPE_1);

    otapp_deviceNameSet(deviceName_device1, UT_DN_OK_DEVICE_TYPE_2_SWITCH);
    result = otapp_deviceNameIsMatching(_deviceNameFull);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_IS, result);
}

TEST(ot_app_deviceName, GivenNotMatchingDeviceNameFull_WhenIsCallingDeviceNameIsMatching_ThenReturnError)
{
    int8_t result;
    char *_deviceNameFull = ut_dn_createDeviceNameFull(deviceName_device2, UT_DN_OK_DEVICE_TYPE_1);

    otapp_deviceNameSet(deviceName_device1, UT_DN_OK_DEVICE_TYPE_2_SWITCH);
    result = otapp_deviceNameIsMatching(_deviceNameFull);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_IS_NOT, result);
}

// otapp_deviceNameFullToEUI()
TEST(ot_app_deviceName, GivenNullArgs_WhenIsCallingDeviceNameFullToEUI_ThenReturnError)
{
    int8_t result;   
    char *EuiPtrStr = NULL;

    result = otapp_deviceNameFullToEUI(NULL, 5, &EuiPtrStr);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
    
    result = otapp_deviceNameFullToEUI(deviceNameFull_device1_type0_fakeAddr, 0, &EuiPtrStr);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
}

TEST(ot_app_deviceName, GivenTooLongDevNameArgs_WhenIsCallingDeviceNameFullToEUI_ThenReturnERROR)
{
    int8_t result;   
    char *EuiPtrStr = NULL;

    result = otapp_deviceNameFullToEUI(deviceNameFull_device1_type0_fakeAddr_too_long, strlen(deviceNameFull_device1_type0_fakeAddr_too_long), &EuiPtrStr);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
    
}

TEST(ot_app_deviceName, GivenTooShortDevNameArgs_WhenIsCallingDeviceNameFullToEUI_ThenReturnERROR)
{
    int8_t result;   
    char *EuiPtrStr = NULL;

    result = otapp_deviceNameFullToEUI(deviceNameFull_device1_type0_fakeAddr_too_short, strlen(deviceNameFull_device1_type0_fakeAddr_too_short), &EuiPtrStr);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_ERROR, result);
    
}

TEST(ot_app_deviceName, GivenTrueArgs_WhenIsCallingDeviceNameFullToEUI_ThenReturnOK)
{
    int8_t result;   
    char *EuiPtrStr = NULL;

    result = otapp_deviceNameFullToEUI(deviceNameFull_device1_type0_fakeAddr, strlen(deviceNameFull_device1_type0_fakeAddr), &EuiPtrStr);
    TEST_ASSERT_EQUAL(OTAPP_DEVICENAME_OK, result);
    TEST_ASSERT_EQUAL_STRING(deviceNameFull_device1_type0_fakeAddr_eui, EuiPtrStr);
    
}