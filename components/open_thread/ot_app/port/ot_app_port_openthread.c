


#include "ot_app_port_openthread.h"

#ifdef ESP_PLATFORM
    void otapp_port_openthread_start(otOperationalDatasetTlvs *datasetTlvs)
    {
        esp_openthread_auto_start(datasetTlvs);
    }

#elif defined(STM_PLATFORM)

    void otapp_port_openthread_start(otOperationalDatasetTlvs *datasetTlvs)
    {
        
    }

#endif