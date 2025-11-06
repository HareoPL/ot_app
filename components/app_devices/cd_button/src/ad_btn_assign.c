/**
 * @file ad_btn_assign.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief button function assignment
 * @version 0.1
 * @date 28-10-2025
 * 
 * @copyright The MIT License (MIT) Copyright (c) 2025 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 * 
 */
#include "ad_btn_assign.h"
#include "ad_button.h"
#include "ad_btn_dimControl.h"

#include "softTimers.h"
#include "OneButton.h"

#include "string.h"


#define OT_BTN_MAX_RGB_SWITCH_COLOR 10
#define OT_AD_BTN_ASSIGN_TIME_MS    40000 

#define OT_BTN_EUI_CHAR_MAX_SIZE    (3 * OT_EXT_ADDRESS_SIZE)       // numbers from range of 0 to 255. OT_EXT_ADDRESS_SIZE is uint8_t. When we need save num as a char, uint8_t a = 255, in char it will be 3 byte(2 - 1chr, 5 - 2chr, 5 - 3chr)
#define OT_BTN_EUI_CHAR_MIN_SIZE    (1 * OT_EXT_ADDRESS_SIZE)       // numbers from range of 0 to 9
#define OT_BTN_EUI_BUF_SIZE         (OT_BTN_EUI_CHAR_MAX_SIZE + 1)  // + 1 EOL

#define OT_BTN_GPIO_QTY     AD_BUTTON_NUM_OF_BUTTONS

static const char ot_btn_charEmptyBuffer = '\0';

typedef struct {
    gpio_num_t       gpioNum;
    uint8_t         isMarkedAssign    : 1;  // button marked to be assigned to a device
}ad_btn_btnIteams_t;

typedef struct{
    otapp_pair_Device_t *dev;
    char        eui[OT_BTN_EUI_BUF_SIZE]; 
    ad_btn_btnIteams_t  btn;
    uint8_t         isTaken             : 1;  // flag of availability on the list
    
}ad_btn_t;

typedef struct {
    uint8_t counter;
    uint8_t isLongPress : 1;
}ad_btn_reset_t;

static ad_btn_reset_t ad_btn_resHandle;
static ad_btn_t btnList[AD_BUTTON_NUM_OF_BUTTONS];
static OneButton_t ot_btn_OB_handleList[OT_BTN_GPIO_QTY];
static ot_app_devDrv_t *drv;
static softTim_t ad_btn_assignTime;

typedef uint32_t (*ad_btn_uriState_callback)(uint8_t btnListId, otapp_deviceType_t uriDevType);
void ad_btn_coapResHandle(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo, otError aResult){}

void ad_btn_task(void)
{   
    for (uint16_t i = 0; i < OT_BTN_GPIO_QTY; i++)
    {
        OneButtonTask(&ot_btn_OB_handleList[i]);
    }    

    SoftTim_eventTask(&ad_btn_assignTime);
}

static int8_t ad_btn_getBtnId(uint16_t gpioNum)
{
    for (uint8_t i = 0; i < AD_BUTTON_NUM_OF_BUTTONS; i++)
    {
        if(btnList[i].btn.gpioNum == gpioNum)
        {
            return i;
        }
    }
    
    return AD_BUTTON_ERROR;
}

static int8_t ad_btn_isTaken(uint8_t btnListId)
{
    if(btnListId >= AD_BUTTON_NUM_OF_BUTTONS) return AD_BUTTON_ERROR;

    return btnList[btnListId].isTaken;
}

static int8_t ad_btn_NvsClearOne(uint8_t btnListId)
{
    int8_t result;
    if(btnListId >= AD_BUTTON_NUM_OF_BUTTONS) return AD_BUTTON_ERROR;

    result = drv->api.nvs.saveString(&ot_btn_charEmptyBuffer, btnListId);

    return result;
}

int8_t ad_btn_setDefaultSettings(void)
{
    for (uint8_t i = 0; i < AD_BUTTON_NUM_OF_BUTTONS; i++)
    {
        // save 0 into NVS
        ad_btn_NvsClearOne(i);

        // set 0 into btnList
        btnList[i].btn.isMarkedAssign = 0;
        btnList[i].dev = NULL;
        btnList[i].eui[0] = ot_btn_charEmptyBuffer;
        btnList[i].isTaken = 0;
    }
        
    return AD_BUTTON_OK;
}

////////////////////////////////
// resetSettings functions

static int8_t ad_btn_resetLongPressIs(ad_btn_reset_t *resHandle)
{
    if(resHandle == NULL) return AD_BUTTON_ERROR;
    
    return resHandle->isLongPress;
}

static int8_t ad_btn_resetLongPressSetTrue(ad_btn_reset_t *resHandle)
{
    if(resHandle == NULL) return AD_BUTTON_ERROR;

    resHandle->isLongPress = 1;
    return AD_BUTTON_OK;
}

static int8_t ad_btn_resetCountAdd(ad_btn_reset_t *resHandle)
{
    if(resHandle == NULL) return AD_BUTTON_ERROR;

    resHandle->counter ++;    
    return AD_BUTTON_OK;
}

static int8_t ad_btn_resetDefaultsetHandle(ad_btn_reset_t *resHandle)
{
    if(resHandle == NULL) return AD_BUTTON_ERROR;

    resHandle->counter = 0;
    resHandle->isLongPress = 0;

    return AD_BUTTON_OK;
}

static int8_t ad_btn_resetFactory(ad_btn_reset_t *resHandle)
{
    if(resHandle == NULL) return AD_BUTTON_ERROR;

    ad_btn_resetDefaultsetHandle(resHandle);
    ad_btn_setDefaultSettings();     

    return AD_BUTTON_OK;
}

static int8_t ad_btn_resetStart(ad_btn_reset_t *resHandle, uint16_t gpioNum, uint8_t obEvent)
{
    if(resHandle == NULL) return AD_BUTTON_ERROR;

    if(gpioNum == OT_BTN_RESET_LONGPRESS_GPIO && obEvent == OT_BTN_OB_LONG_PRESS)
    {
        if(ad_btn_resetLongPressIs(resHandle) == 0) // check if isLongPress == 0
        {
            ad_btn_resetLongPressSetTrue(resHandle); // set isLongPress == 1

            SoftTim_start(&ad_btn_assignTime, OT_BTN_RESET_TIME_MS);

            printf("reset: start \n");   
            return AD_BUTTON_IS_IN_RESET_STATE;        
        }
    }
    else if(gpioNum == OT_BTN_RESET_ONE_CLICK_GPIO && obEvent == OT_BTN_OB_ONE_CLICK)
    {
        // check if isLongPress == 1
        if(ad_btn_resetLongPressIs(resHandle))
        {
            ad_btn_resetCountAdd(resHandle);
            
            // check if counter equal OT_BTN_RESET_OC_CNT_RESET
            if(resHandle->counter >= OT_BTN_RESET_OC_CNT_RESET)
            {
                SoftTim_stop(&ad_btn_assignTime);
                ad_btn_resetFactory(resHandle); // do reset factory settings 
                printf("factory reset has been done \n");
            }            
            
            return AD_BUTTON_IS_IN_RESET_STATE;
        }
    }
    return AD_BUTTON_IS_NOT;
}

static int8_t ad_btn_resetStop(ad_btn_reset_t *resHandle, uint16_t gpioNum)
{
    if(resHandle == NULL) return AD_BUTTON_ERROR;

    if(gpioNum == OT_BTN_RESET_LONGPRESS_GPIO)
    {
        if(ad_btn_resetLongPressIs(resHandle))
        {
            ad_btn_resetDefaultsetHandle(resHandle);
            
            SoftTim_stop(&ad_btn_assignTime);
            printf("reset and timer: stop \n");
        }        
    }
    
    return AD_BUTTON_OK;
}

////////////////////////////////
// EUI functions

// return btnListId or IS_NOT 
static int8_t ad_btn_EuiIsExist(const char *devNameFull)
{
    if(devNameFull == NULL) return AD_BUTTON_ERROR;

    for (uint8_t i = 0; i < AD_BUTTON_NUM_OF_BUTTONS; i++)
    {
        if(drv->api.devName.devNameEuiIsSame(devNameFull, btnList[i].eui) == OTAPP_DEVICENAME_IS)
        {
            return i;
        }
    }
    
    return AD_BUTTON_IS_NOT;
}

static int8_t ad_btn_EuiReadFromNvs(uint8_t btnListId)
{
    int8_t result;
    if(btnListId >= AD_BUTTON_NUM_OF_BUTTONS) return AD_BUTTON_ERROR;

    result = drv->api.nvs.readString(btnList[btnListId].eui, OT_BTN_EUI_BUF_SIZE, btnListId);
    if(result == AD_BUTTON_ERROR) return AD_BUTTON_ERROR;

    return AD_BUTTON_OK;
}

static int8_t ad_btn_EuiSaveToNvs(char *eui, uint8_t btnListId)
{
    int8_t result;
    // char nvsBuf[OT_BTN_EUI_BUF_SIZE];

    if(eui == NULL) return AD_BUTTON_ERROR;    
    
    result = drv->api.nvs.saveString(btnList[btnListId].eui, btnListId); // always btnListId == gpioNum it was assigned in init function

    return result; // -1 OK
}

static int8_t ad_btn_EuiSave(const char *devNameFull, uint8_t btnListId)
{
    char *devEui;
    int8_t result;
    if(devNameFull == NULL) return AD_BUTTON_ERROR;

    result = drv->api.devName.devNameFullToEUI(devNameFull, strlen(devNameFull), &devEui);
    if(result == OTAPP_DEVICENAME_ERROR) return AD_BUTTON_ERROR;

    strcpy(btnList[btnListId].eui, devEui);
    ad_btn_EuiSaveToNvs(devEui, btnListId);
    
    return AD_BUTTON_OK;
}

static int8_t ad_btn_EuiIsEmpty(uint8_t btnListId)
{   
    if(btnListId >= AD_BUTTON_NUM_OF_BUTTONS) return AD_BUTTON_ERROR;
    
    if(btnList[btnListId].eui[0] == ot_btn_charEmptyBuffer)
    {
        return AD_BUTTON_IS;    
    }

    return AD_BUTTON_IS_NOT;
}

static void ad_btn_settingsLoad(void)
{
    for (uint8_t i = 0; i < AD_BUTTON_NUM_OF_BUTTONS; i++)
    {
        ad_btn_EuiReadFromNvs(i);        
    }  
}

////////////////////////////////
// assign functions
static int8_t ad_btn_assignSetTrue(uint8_t btnListId)
{
    if(btnListId >= AD_BUTTON_NUM_OF_BUTTONS) return AD_BUTTON_ERROR;

    btnList[btnListId].btn.isMarkedAssign = 1;
    SoftTim_start(&ad_btn_assignTime, OT_AD_BTN_ASSIGN_TIME_MS);

    printf("assignTimer started \n");

    return AD_BUTTON_OK;
}

static int8_t ad_btn_assignSetFalse(uint8_t btnListId)
{
    if(btnListId >= AD_BUTTON_NUM_OF_BUTTONS) return AD_BUTTON_ERROR;

    btnList[btnListId].btn.isMarkedAssign = 0;

    return AD_BUTTON_OK;
}

static int8_t ad_btn_assignGetTrueMarked(void)
{
    for (uint8_t i = 0; i < AD_BUTTON_NUM_OF_BUTTONS; i++)
    {
        if(btnList[i].btn.isMarkedAssign)
        {
            return i;
        }
    }

    return AD_BUTTON_IS_NOT;    
}

static void ad_btn_assignSetFalseAll(void)
{
    for (uint8_t i = 0; i < AD_BUTTON_NUM_OF_BUTTONS; i++)
    {
        ad_btn_assignSetFalse(i);
    }
    
    ad_btn_resetStop(&ad_btn_resHandle, OT_BTN_RESET_LONGPRESS_GPIO); // stop reset sequence
    printf("assignTimer stoped \n");    
}

static int8_t ad_btn_assignNewDeviceToBtnList(otapp_pair_Device_t *newDevice)
{
    int8_t btnListId;
    if(newDevice == NULL) return AD_BUTTON_ERROR;

    // detect whitch button is ready to assign newDevice. if not error. return btnListId
    btnListId = ad_btn_assignGetTrueMarked();
    if(btnListId == AD_BUTTON_IS_NOT) return AD_BUTTON_ERROR;

    btnList[btnListId].dev = newDevice;                 // assign a newDevice information pointer
    ad_btn_EuiSave(newDevice->devNameFull, btnListId);
    btnList[btnListId].isTaken = 1;

    ad_btn_assignSetFalse(btnListId);     

    return AD_BUTTON_OK;
}

int8_t ad_btn_assignDevice(otapp_pair_Device_t *newDevice)
{    
    int8_t btnListId, result;

    if(newDevice == NULL) return AD_BUTTON_ERROR;
    
    // check if EUI is existed - loaded from NVS 
    btnListId = ad_btn_EuiIsExist(newDevice->devNameFull);
    if(btnListId == AD_BUTTON_ERROR) return AD_BUTTON_ERROR;

    if(btnListId == AD_BUTTON_IS_NOT) // assigned new device to button
    {
        result = ad_btn_assignNewDeviceToBtnList(newDevice);
        if(result == AD_BUTTON_ERROR) return AD_BUTTON_ERROR;

        SoftTim_stop(&ad_btn_assignTime);
        printf("Assigned new device | Stop timer \n");
        // assign new devica has been finished succesfull. 
        // todo-future let's togle light coap send togle 
    }
    else // return index of existed Eui on the list. eui Device was saved into NVS
    {
        if(ad_btn_isTaken(btnListId) == 0)
        {
            btnList[btnListId].dev = newDevice;
            btnList[btnListId].isTaken = 1;
        }        
    }

    return AD_BUTTON_OK;
}

////////////////////////////////
// uri functions

static uint32_t ad_btn_uriStateGet(uint8_t btnListId, otapp_deviceType_t uriDevType)
{
    uint8_t uriListId = 0;
    if(btnListId >= AD_BUTTON_NUM_OF_BUTTONS) return 255;

    uriListId = drv->api.pair.uriGetIdList(btnList[btnListId].dev, uriDevType);

    return btnList[btnListId].dev->urisList[uriListId].uriState;
}

static int8_t ad_btn_uriStateSet(uint8_t btnListId, otapp_deviceType_t uriDevType, uint32_t *uriState)
{
    uint8_t uriListId = 0;
    if(btnListId >= AD_BUTTON_NUM_OF_BUTTONS || uriState == NULL) return AD_BUTTON_ERROR;
    
    uriListId = drv->api.pair.uriGetIdList(btnList[btnListId].dev, uriDevType);

    btnList[btnListId].dev->urisList[uriListId].uriState = *uriState;
    return AD_BUTTON_OK;
}

static uint32_t ad_btn_uriStateSetNewTogle(uint8_t btnListId, otapp_deviceType_t uriDevType)
{    
    uint32_t uriState_ = 0;

    uriState_ = !(ad_btn_uriStateGet(btnListId, uriDevType)); // we immediately negation of value

    ad_btn_uriStateSet(btnListId, uriDevType, &uriState_);
    
    return uriState_;
}

static uint32_t ad_btn_uriStateSetNewValue(uint8_t btnListId, otapp_deviceType_t uriDevType, uint16_t maxValue, uint8_t stepValue)
{   
    uint32_t uriState_ = 0;   
    
    uriState_ = ad_btn_uriStateGet(btnListId, uriDevType);    
    uriState_ = (uriState_ + stepValue) % maxValue;

    ad_btn_uriStateSet(btnListId, uriDevType, &uriState_);

    return uriState_;
}

static uint32_t ad_btn_uriStateSetNewDimm(uint8_t btnListId, otapp_deviceType_t uriDevType)
{   
    uint32_t newDimVal;
    uint32_t currentDimVal;

    currentDimVal = ad_btn_uriStateGet(btnListId, uriDevType);

    newDimVal = ad_btn_dim_GetNewValue(currentDimVal);
    ad_btn_uriStateSet(btnListId, uriDevType, &newDimVal);

    return newDimVal;
}

static uint32_t ad_btn_uriStateSetNewRgb(uint8_t btnListId, otapp_deviceType_t uriDevType)
{   
    return ad_btn_uriStateSetNewValue(btnListId, uriDevType, OT_BTN_MAX_RGB_SWITCH_COLOR, 1);
}

static void ad_btn_coapSend(uint8_t btnListId, uint32_t *newState, otapp_deviceType_t uriDevType)
{
    otIp6Address *ipAddr;
    char *uriPath;
    uint8_t uriListId;

    uriListId = drv->api.pair.uriGetIdList(btnList[btnListId].dev, uriDevType);
    ipAddr = &btnList[btnListId].dev->ipAddr;
    uriPath = btnList[btnListId].dev->urisList[uriListId].uri;

    // send coap message
    drv->api.coap.sendBytePut(ipAddr,uriPath, (uint8_t*)newState, sizeof(newState), ad_btn_coapResHandle, NULL);    
}

static int8_t ad_btn_event(uint16_t gpioNum, otapp_deviceType_t uriDevType, ad_btn_uriState_callback uriStateClb, char * btnName)
{
    int8_t btnListId, result;
    uint32_t newState; 

    btnListId = ad_btn_getBtnId(gpioNum);
    if(btnListId == AD_BUTTON_ERROR) return AD_BUTTON_ERROR;

    // check if button is taken.
    result = ad_btn_isTaken(btnListId);
    if(result == AD_BUTTON_ERROR) return AD_BUTTON_ERROR;
    
    if(result) // true == 1
    {
        // togle light and save uriState
        newState = uriStateClb(btnListId, uriDevType);

        ad_btn_coapSend(btnListId, &newState, uriDevType);

        printf("%s: send state: %ld  \n",btnName, newState);
    }
    else
    {
        printf("%s device is not turned on or not assigned \n", btnName);
    }  

    return AD_BUTTON_OK;
}

static void ad_btn_eventOneClick(uint16_t gpioNum)
{
    int8_t btnListId, result;
    uint32_t newState; 

    btnListId = ad_btn_getBtnId(gpioNum);
    if(btnListId == AD_BUTTON_ERROR) return;

    result = ad_btn_EuiIsEmpty(btnListId);
    if(result == AD_BUTTON_ERROR) return;

    if(result == AD_BUTTON_IS) // btn is not assigned. let's marked it 
    {
        if(ad_btn_assignSetTrue(btnListId) == AD_BUTTON_ERROR) return;
    }
    else // AD_BUTTON_IS_NOT
    {   
        // check if button is taken.
        result = ad_btn_isTaken(btnListId);
        if(result == AD_BUTTON_ERROR) return ;
        
        if(result) // true == 1
        {
            // togle light and save uriState
            newState = ad_btn_uriStateSetNewTogle(btnListId, OTAPP_LIGHTING_ON_OFF); 

            ad_btn_coapSend(btnListId, &newState, OTAPP_LIGHTING_ON_OFF);

            printf("oneClick: send state: %ld  \n", newState);
        }
        else
        {
            printf("oneClick device is not turned on or not assigned  \n");
        }
    }   
}

////////////////////////////////
// OneButton functions

static void ad_btn_oneClick(uint16_t gpioNum)
{    
    if(ad_btn_resetStart(&ad_btn_resHandle, gpioNum, OT_BTN_OB_ONE_CLICK) != AD_BUTTON_IS_IN_RESET_STATE)
    {
        ad_btn_eventOneClick(gpioNum);
    }
 
}

static void ad_btn_doubleClick(uint16_t gpioNum)
{
    ad_btn_event(gpioNum, OTAPP_LIGHTING_RGB, ad_btn_uriStateSetNewRgb, "doubleClick");
}

static void ad_btn_longPressStart(uint16_t gpioNum)
{    
    ad_btn_resetStart(&ad_btn_resHandle, gpioNum, OT_BTN_OB_LONG_PRESS);    
    ad_btn_event(gpioNum, OTAPP_LIGHTING_DIMM, ad_btn_uriStateSetNewDimm, "longPress");    
}

static void ad_btn_longPressStop(uint16_t gpioNum)
{    
    ad_btn_resetStop(&ad_btn_resHandle, gpioNum); // stop reset sequence
}


////////////////////////////////
// init functions

static void ad_btn_initGpio(void)
{
    for (uint8_t i = 0; i < OT_BTN_GPIO_QTY; i++)
    {
        gpio_reset_pin(ot_btn_gpioList[i]);                         // reset to default state
        gpio_set_direction(ot_btn_gpioList[i], GPIO_MODE_INPUT);    // set gpio as input    
        gpio_pullup_en(ot_btn_gpioList[i]);                         // set pull-up

        btnList[i].btn.gpioNum = ot_btn_gpioList[i];                // assigned GPIO num from gpioList. it is phisical GPIO selected 
    }   
}

static void ad_btn_initOnoButton()
{
    for (uint16_t i = 0; i < OT_BTN_GPIO_QTY; i++)
    {
        OneButtonInit(&ot_btn_OB_handleList[i], ot_btn_gpioList[i]);
        OneButtonCallbackOneClick(&ot_btn_OB_handleList[i], ad_btn_oneClick);
        OneButtonCallbackDoubleClick(&ot_btn_OB_handleList[i], ad_btn_doubleClick);
        OneButtonCallbackLongPressStart(&ot_btn_OB_handleList[i], ad_btn_longPressStart); // the function will repeat itself until you release the button
        OneButtonCallbackLongPressStop(&ot_btn_OB_handleList[i], ad_btn_longPressStop);
    }       
}

int8_t ad_btn_init(ot_app_devDrv_t *drvPtr)
{
    if(drvPtr == NULL) return AD_BUTTON_ERROR;
    drv = drvPtr;

    ad_btn_initGpio();
    ad_btn_initOnoButton();   
    OneButtonSetTimerLongPressTick(&ot_btn_OB_handleList[1], 100); //  // ot_btn_gpioList GPIO_9

    // load eui from nvs
    ad_btn_settingsLoad();
    SoftTim_init(&ad_btn_assignTime, ad_btn_assignSetFalseAll, 1);

    return AD_BUTTON_OK; 
}
// todo actory_reset_procedure