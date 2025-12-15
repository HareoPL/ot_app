#ifndef MOCK_MOCKS_H_
#define MOCK_MOCKS_H_

#include "stdint.h"

#define OT_EXT_ADDRESS_SIZE 8 

struct otExtAddress
{
    uint8_t m8[OT_EXT_ADDRESS_SIZE]; ///< IEEE 802.15.4 Extended Address bytes
} OT_TOOL_PACKED_END;

/**
 * Represents the IEEE 802.15.4 Extended Address.
 */
typedef struct otExtAddress otExtAddress;


const otExtAddress *otapp_macAddrGet(void);





#endif  /* MOCK_MOCKS_H_ */


