/**
 * @file srv_app.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 21-04-2025
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
#include "main.h"
#include "web_app.h"

#include "esp_http_server.h"
#include <sys/stat.h>
#include <sys/unistd.h>
#include "esp_spiffs.h"
#include "spiffs.h"
#include "cJSON.h"

static const char *TAG = "web_app";

static httpd_handle_t web_app_server;

static char web_app_buffer[512];

static uint8_t web_app_spiffsIsMounted = 0;

/************************************ 
 * URLs HANDLERS 
 */

/******* index HANDLER */
esp_err_t web_app_handlerURL_index(httpd_req_t *req) 
{
    FILE *fd = NULL;
    char *buf = NULL;
    struct stat st;

    if(web_app_spiffsIsMounted == 0)
    {
         const char* resp =
            "<html><head>"
            "<meta http-equiv=\"refresh\" content=\"3\">"
            "</head><body>"
            "Initializing of web_app module. Pleace wait...<br>"
            "In a moment the page will be automatically refreshed."
            "</body></html>";
        httpd_resp_set_type(req, "text/html");
        httpd_resp_sendstr(req, resp);

        if(spiffs_mount() == ESP_OK)
        {
            web_app_spiffsIsMounted = 1;
        }
        else
        {
            ESP_LOGE(TAG, "spiffs ERROR:");
            const char* resp = "<html><head></head><body> Initializing of SPIFFS ERROR.</body></html>";
            httpd_resp_set_type(req, "text/html");
            httpd_resp_sendstr(req, resp);
        }
    }

    const char* filename = "/spiffs/index.html";
       
    // open file
    if ((fd = fopen(filename, "rb")) == NULL)  // open file read and binary
    {
        ESP_LOGE(TAG, "can not open file: %s", filename);
        
        sprintf(web_app_buffer, "ERROR in file: %s, line: %d", __FILE__, __LINE__);
        httpd_resp_send_custom_err(req, "404 Not Found", web_app_buffer);
        return ESP_FAIL;
    }

    // File size download
    if (stat(filename, &st) != 0) 
    {
        ESP_LOGE(TAG, "Stat error for file %s", filename);

        sprintf(web_app_buffer, "ERROR in file: %s, line: %d", __FILE__, __LINE__);
        httpd_resp_send_custom_err(req, "404 Not Found", web_app_buffer);
        fclose(fd);
        return ESP_FAIL;
    }
    size_t file_size = st.st_size;

    // Buffer allocation
    buf = (char*)malloc(file_size);
    if (NULL == buf) 
    {
        ESP_LOGE(TAG, "Memory allocation error");
        fclose(fd);

        sprintf(web_app_buffer, "ERROR in file: %s, line: %d", __FILE__, __LINE__);
        httpd_resp_send_custom_err(req, "404 Not Found", web_app_buffer);
        return ESP_FAIL;
    }

    // Loading the file contents
    if (fread(buf, 1, file_size, fd) != file_size) 
    {
        ESP_LOGE(TAG, "File reading error");
        free(buf);
        fclose(fd);

        sprintf(web_app_buffer, "ERROR in file: %s, line: %d", __FILE__, __LINE__);
        httpd_resp_send_custom_err(req, "404 Not Found", web_app_buffer);
        return ESP_FAIL;
    }

    // Sending HTTP answers
    httpd_resp_set_type(req, "text/html");
    esp_err_t ret = httpd_resp_send(req, buf, file_size);

    // Resource release
    free(buf);
    fclose(fd);
    
    return ret;
}


/******** api HANDLER */
static esp_err_t web_app_handlerURL_api(httpd_req_t *req) 
{
    int received;

    // receive data
    received = httpd_req_recv(req, web_app_buffer, sizeof(web_app_buffer));
    if (received <= 0) 
    {
        if (received == HTTPD_SOCK_ERR_TIMEOUT) 
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    web_app_buffer[received] = '\0'; // Null-terminate

    // parse JSON
    cJSON *root = cJSON_Parse(web_app_buffer);
    if (root == NULL) 
    {
        ESP_LOGE(TAG, "parse error JSON");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Incorrect JSON");
        return ESP_FAIL;
    }

    // Download values ​​from JSON
    cJSON *sensor = cJSON_GetObjectItem(root, "sensor");
    cJSON *value = cJSON_GetObjectItem(root, "value");
    
    if (!cJSON_IsString(sensor) || !cJSON_IsNumber(value)) 
    {
        ESP_LOGE(TAG, "Incorrect JSON structure");
        cJSON_Delete(root);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "No fields required");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "receive: sensor=%s, value=%.2f", sensor->valuestring, value->valuedouble);
    
    // Return the answer
    httpd_resp_set_type(req, "application/json");
    const char *resp = "{\"status\": \"OK\"}";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    // memory free
    cJSON_Delete(root);
    return ESP_OK;
}


static esp_err_t web_app_handlerURL_test(httpd_req_t *req) 
{
    httpd_resp_set_type(req, "text/html");
    uint8_t bufferSize = sprintf(web_app_buffer, "test server: %s, line: %d", __FILE__, __LINE__);
    esp_err_t ret = httpd_resp_send(req, web_app_buffer, bufferSize);
    return ret;
}
/* 
 * END OF URLs HANDLERS 
 ************************************/

/************************************ 
 * Register URLS
 */ 
void web_app_registerURL_api(void)
{
    httpd_uri_t post_uri = {
            .uri       = "/api",
            .method    = HTTP_POST,
            .handler   =  web_app_handlerURL_api,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(web_app_server, &post_uri);
}

void web_app_registerURL_index(void)
{
    httpd_uri_t uri_get = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = web_app_handlerURL_index,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(web_app_server, &uri_get);
}

void web_app_registerURL_test(void)
{
    httpd_uri_t uri_get = {
        .uri       = "/test",
        .method    = HTTP_GET,
        .handler   = web_app_handlerURL_test,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(web_app_server, &uri_get);
}
/*
 * END OF Register URLS
 ************************************/ 


//  static esp_err_t hello_get_handler(httpd_req_t *req) {
//     const char resp[] = "Hello World!";
//     httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
//     return ESP_OK;
// }

// static httpd_uri_t hello = {
//     .uri       = "/hello",
//     .method    = HTTP_GET,
//     .handler   = hello_get_handler,
//     .user_ctx  = NULL
// };

// void start_webserver(void) {
//     httpd_config_t config = HTTPD_DEFAULT_CONFIG();
//     httpd_handle_t server = NULL;
//     if (httpd_start(&server, &config) == ESP_OK) {
//         httpd_register_uri_handler(server, &hello);
//     }
// }


void web_app_startWebServer(void)
{
    httpd_config_t web_app_config = HTTPD_DEFAULT_CONFIG();
    // start_webserver();
    

    if (httpd_start(&web_app_server, &web_app_config) == ESP_OK) 
    {
        web_app_registerURL_api();  
        web_app_registerURL_index();
        web_app_registerURL_test();
    }
}