/**
 * @file spiffs.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 17-04-2025
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

#include "spiffs.h"

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_spiffs.h"


static const char *TAG = "example";
static esp_err_t errorStat;

TaskHandle_t th_Spiffs = NULL;
UBaseType_t stackFree_spiffsTask;

static uint8_t spiffs_IsMounted = 0;

static esp_vfs_spiffs_conf_t spiffsConf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true
  };

  

esp_err_t spiffs_checkOnStart(void)
{
    ESP_LOGI(TAG, "Performing SPIFFS_check().");

    errorStat = esp_spiffs_check(spiffsConf.partition_label);

    if (errorStat != ESP_OK) 
    {
        ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(errorStat));

        return errorStat;
    } else 
    {
        ESP_LOGI(TAG, "SPIFFS_check() successful");
    }

    return ESP_OK;
}

esp_err_t spiffs_mount(void)
{
    if(spiffs_IsMounted == 0)
    {
       ESP_LOGI(TAG, "Initializing SPIFFS");

        errorStat = esp_vfs_spiffs_register(&spiffsConf);

        if (errorStat != ESP_OK) 
        {
            if (errorStat == ESP_FAIL) 
            {
                ESP_LOGE(TAG, "Failed to mount or format filesystem");
            } 
            else if (errorStat == ESP_ERR_NOT_FOUND) 
            {
                ESP_LOGE(TAG, "Failed to find SPIFFS partition");
            } 
            else 
            {
                ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(errorStat));
            }
            return errorStat;
        }

        #ifdef SPIFFS_CHECK_ON_START
        errorStat = spiffs_checkOnStart();
        #endif

        spiffs_IsMounted = 1;
        return ESP_OK;
    }
    else
    {
        ESP_LOGI(TAG, "SPIFFS was mount");
    }

    return ESP_OK;
}

esp_err_t spiffs_partitionSizeInfo(size_t *totalSize, size_t *usedSize)
{
    size_t total = 0, used = 0;

    errorStat = esp_spiffs_info(spiffsConf.partition_label, &total, &used);

    if (errorStat != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(errorStat));

        esp_spiffs_format(spiffsConf.partition_label);
        
        return errorStat;
    } else 
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    // Check consistency of reported partition size info.
    if (used > total) 
    {
        ESP_LOGW(TAG, "Number of used bytes cannot be larger than total. Performing SPIFFS_check().");

        errorStat = esp_spiffs_check(spiffsConf.partition_label);

        // Could be also used to mend broken files, to clean unreferenced pages, etc.
        // More info at https://github.com/pellepl/spiffs/wiki/FAQ#powerlosses-contd-when-should-i-run-spiffs_check
        if (errorStat != ESP_OK) 
        {
            ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(errorStat));
            return errorStat;
        } else 
        {
            ESP_LOGI(TAG, "SPIFFS_check() successful");
        }
    }

    *totalSize = total;
    *usedSize = used;

    return errorStat;
}

esp_err_t spiffs_unmount(void)
{
    errorStat = esp_vfs_spiffs_unregister(spiffsConf.partition_label);

    if(errorStat != ESP_OK)
    {
        ESP_LOGI(TAG, "SPIFFS unmounted ERROR");
        return errorStat;
    }
    else
    {
        spiffs_IsMounted = 0;
        ESP_LOGI(TAG, "SPIFFS unmounted");
    }

    return errorStat;
}



///////////////////////////
void spiffs_runTest(void)
{
   
    // Use POSIX and C standard library functions to work with files.
    // First create a file.
    ESP_LOGI(TAG, "Opening file");
    FILE* f = fopen("/spiffs/hello.txt", "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, "Hello World!\n");
    fclose(f);
    ESP_LOGI(TAG, "File written");

    
    // Check if destination file exists before renaming
    struct stat st;
    if (stat("/spiffs/foo.txt", &st) == 0) 
    {
        // Delete it if it exists
        unlink("/spiffs/foo.txt");
    }

    // Rename original file
    ESP_LOGI(TAG, "Renaming file");
    if (rename("/spiffs/hello.txt", "/spiffs/foo.txt") != 0) {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }

    // Open renamed file for reading
    ESP_LOGI(TAG, "Reading file");
    f = fopen("/spiffs/foo.txt", "r");
    if (f == NULL) 
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }

    char line[64];

    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    char* pos = strchr(line, '\n');
    if (pos) 
    {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);
}

void spiffsTask(void *pvParameter)
{
    size_t totalSize_ = 0;
    size_t usedSize_ = 0;

    while (1) 
    {
        stackFree_spiffsTask = uxTaskGetStackHighWaterMark(NULL);
 
        spiffs_mount();
        spiffs_partitionSizeInfo(&totalSize_, &usedSize_);
       
        ESP_UNUSED(totalSize_);
        ESP_UNUSED(usedSize_);
     

        spiffs_runTest();
        spiffs_unmount();

        // vTaskSuspend(th_Spiffs);
        vTaskDelete(th_Spiffs);

        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}

void spiffs_test(void)
{
    xTaskCreate(spiffsTask, "spiffs", 1024 * 8, NULL, 5, &th_Spiffs);
}
