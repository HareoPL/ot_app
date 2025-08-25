/**
 * @file ot_app_pair.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 19-08-2025
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
#ifndef OT_APP_PAIR_H_
#define OT_APP_PAIR_H_

#include "hro_utils.h"

#ifndef UNIT_TEST
    #include "ot_app.h"
#else
    #include "mock_ot_app.h" 
         
#endif

#define OTAPP_PAIR_IS                       (1)
#define OTAPP_PAIR_IS_NOT                   (0)
#define OTAPP_PAIR_UPDATED                  (OTAPP_PAIR_IS)
#define OTAPP_PAIR_NO_NEED_UPDATE           (OTAPP_PAIR_IS_NOT)

#define OTAPP_PAIR_OK                       (-1)
#define OTAPP_PAIR_ERROR                    (-2)
#define OTAPP_PAIR_NO_EXIST                 (-3)
#define OTAPP_PAIR_DEVICE_NAME_EXIST        (-4)
#define OTAPP_PAIR_DEVICE_NAME_TO_LONG      (-5)
#define OTAPP_PAIR_DEVICE_NO_SPACE          (-6)

#define OTAPP_PAIR_DEVICES_MAX    OTAPP_PAIRED_DEVICES_MAX // max number of devices to save them from DNS query
#define OTAPP_PAIR_URI_MAX        OTAPP_PAIRED_URI_MAX 
#define OTAPP_PAIR_URI_MAX_VAL    OTAPP_URI_END_OF_INDEX
#define OTAPP_PAIR_NAME_SIZE      OTAPP_DEVICE_NAME_SIZE 
#define OTAPP_PAIR_NO_URI         OTAPP_URI_NO_URI_INDEX
#define OTAPP_PAIR_URI_INIT       OTAPP_URI_END_OF_INDEX



typedef struct otapp_pair_DeviceList_t otapp_pair_DeviceList_t;

/**
 * @brief add new device to the pairing list
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle()
 * @param deviceNameFull  [in] char ptr of full device name ("device1_1_588c81fffe301ea4")
 * @param ipAddr          [in] ptr to IP address of the new device
 * @return int8_t         [out] table index of pairDeviceList->list[tableIndex] 
 *                              or  OTAPP_PAIR_ERROR, OTAPP_PAIR_DEVICE_NAME_EXIST, OTAPP_PAIR_DEVICE_NAME_TO_LONG
 */
int8_t otapp_pair_DeviceAdd(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull, otIp6Address *ipAddr);

/**
 * @brief get indexDevice from device list
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle()  
 * @param deviceNameFull  [in] char ptr of full device name ("device1_1_588c81fffe301ea4") 
 * @return int8_t         [out] indexDevice 
 *                              or OTAPP_PAIR_ERROR (-1)
 */
int8_t otapp_pair_DeviceIndexGet(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull);

/**
 * @brief get deviceNameFull from device list
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle() 
 * @param indexDevice     [in] index of device 
 * @return char*          [out] ptr to deviceNameFull, 
 *                              or NULL if error
 */
char *otapp_pair_DeviceNameGet(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice);

/**
 * @brief add new uriIndex of paired device. First of all you should add new device using otapp_pair_DeviceAdd()
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle()
 * @param deviceNameFull  [in] char ptr of full device name ("device1_1_588c81fffe301ea4")
 * @param uriIndex        [in] uriIndex from otapp_coap_uriTableIndex_t
 * @return int16_t        [out] return MSB = device index (pairDeviceList->list[]),
 *                                     LSB = uri index (pairDeviceList->list[].uriIndex[]) 
 *                                      or  OTAPP_PAIR_ERROR (-1)
 */
int16_t otapp_pair_DeviceUriIndexAdd(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull, otapp_coap_uriTableIndex_t uriIndex);

/**
 * @brief get URI index from uriIndex of device list 
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle() 
 * @param indexDevice     [in] index of device 
 * @param indexUri        [in] index of URI 
 * @return otapp_coap_uriTableIndex_t [out] otapp_coap_uriTableIndex_t 
 *                                          or OTAPP_PAIR_NO_URI if there is not saved uri
 */
otapp_coap_uriTableIndex_t otapp_pair_deviceUriIndexGet(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice, uint8_t indexUri);

/**
 * @brief delete paired device
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle()
 * @param deviceNameFull  [in] char ptr of full device name ("device1_1_588c81fffe301ea4")
 * @return int8_t         [out] table index of deleted device
 *                              or  OTAPP_PAIR_ERROR (-1) 
 */
int8_t otapp_pair_DeviceDelete(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull);

/**
 * @brief delete all elements in pairDeviceList
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle() 
 * @return int8_t         [out] return OTAPP_PAIR_OK or OTAPP_PAIR_ERROR
 */
int8_t otapp_pair_DeviceDeleteAll(otapp_pair_DeviceList_t *pairDeviceList);

/**
 * @brief get ptr of otapp_pair_DeviceList_t handle
 * 
 * @return otapp_pair_DeviceList_t* [out] ptr of otapp_pair_DeviceList_t handle
 */
otapp_pair_DeviceList_t *otapp_pair_getHandle(void);

/**
 * @brief get saved IP address from device list
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle() 
 * @param indexDevice     [in] index of device 
 * @return otIp6Address*  [out] ptr to otIp6Address, 
 *                              or NULL if error
 */
otIp6Address *otapp_pair_ipAddressGet(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice);

/**
 * @brief check the ip address is same as saved in given device index
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle() 
 * @param indexDevice     [in] index of device  
 * @param ipAddr          [in] ptr to otIp6Address
 * @return int8_t         [out] OTAPP_PAIR_IS, OTAPP_PAIR_IS_NOT
 *                              or OTAPP_PAIR_ERROR, OTAPP_PAIR_NO_EXIST
 */
int8_t otapp_pair_ipAddressIsSame(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice, otIp6Address *ipAddr);

/**
 * @brief update ip address when it is different than saved
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle()  
 * @param indexDevice     [in] index of device  
 * @param ipAddr          [in] ptr to new otIp6Address 
 * @return int8_t         [out] OTAPP_PAIR_UPDATED, OTAPP_PAIR_NO_NEED_UPDATE or if error: OTAPP_PAIR_ERROR, OTAPP_PAIR_NO_EXIST
 */
int8_t otapp_pair_ipAddressUpdate(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice, otIp6Address *ipAddrNew);

/**
 * @brief print all saved data of device
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle() 
 * @param indexDevice     [in] index of device to be print
 */
void otapp_pair_devicePrintData(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice);


#ifdef UNIT_TEST

/**
 * @brief check free space of pairDeviceList->list[]
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle()
 * @return int8_t         [out] free table index of pairDeviceList->list[tableIndex] 
 *                              or  OTAPP_PAIR_ERROR (-1)
 */
PRIVATE int8_t otapp_pair_DeviceIsFreeSpace(otapp_pair_DeviceList_t *pairDeviceList);

/**
 * @brief reserve space in pairing device list 
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle()
 * @param indexDevice     [in] index of free position of pairDeviceList->list[tableIndex] 
 * @note to get free position of device list you should use otapp_pair_DeviceIsFreeSpace()
 */
PRIVATE void otapp_pair_spaceTake(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice);

/**
 * @brief check if space in paring device list is taken
 * 
 * @param pairDeviceList [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle()
 * @param indexDevice    [in] index of device name to check
 * @return int8_t        [out] TRUE = 1, FALSE = 0
 *                             or OTAPP_PAIR_ERROR
 */
PRIVATE int8_t otapp_pair_spaceIsTaken(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice);

/**
 * @brief check if deviceNameFull is existing on pairing device list
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle()
 * @param deviceNameFull  [in] char ptr of full device name ("device1_1_588c81fffe301ea4")
 * @return int8_t         [out] index of device name found in table list
 *                              or  OTAPP_PAIR_ERROR (-1)
 */
PRIVATE int8_t otapp_pair_DeviceIsExist(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull);

/**
 * @brief compare two deviceNameFull
 * 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle() 
 * @param deviceNameFull  [in] char ptr of full device name ("device1_1_588c81fffe301ea4") 
 * @param indexDevice     [in] device index to comparison 
 * @return int8_t         [out] TRUE = 1, FALSE = 0, 
 *                              or OTAPP_PAIR_ERROR (-1) if error
 */
PRIVATE int8_t otapp_pair_deviceNameIsSame(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull, uint8_t indexDevice);

#endif  /* UNIT_TEST */

#endif  /* OT_APP_PAIR_H_ */
