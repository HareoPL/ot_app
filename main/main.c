/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"

#define RMT_LED_STRIP_RESOLUTION_HZ (10 * 1000 * 1000) // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define RMT_LED_STRIP_GPIO_NUM      8

#define EXAMPLE_LED_NUMBERS         3

#define EXAMPLE_FRAME_DURATION_MS   400

#define RMT_TIME_03_US              (0.3 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000) // T=0.3us
#define RMT_TIME_09_US              (0.9 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000) // T=0.9us


static uint8_t led_strip_pixels[EXAMPLE_LED_NUMBERS * 3];

rmt_channel_handle_t tx_chan = NULL;
rmt_encoder_handle_t encoder  = NULL;

rmt_transmit_config_t transmit_cfg = 
{
    .loop_count = 0,
    .flags.eot_level = 0
};

void app_main(void)
{
    rmt_bytes_encoder_config_t bytes_encoder_cfg = 
    {
        .bit0 = {.duration0 = RMT_TIME_03_US, .level0 = 1, .duration1 = RMT_TIME_09_US, .level1 = 0}, // 0.3µs HIGH + 0.9µs LOW
        .bit1 = {.duration0 = RMT_TIME_09_US, .level0 = 1, .duration1 = RMT_TIME_03_US, .level1 = 0}, // 0.9µs HIGH + 0.3µs LOW
    };
    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&bytes_encoder_cfg, &encoder));

    rmt_tx_channel_config_t tx_chan_config = 
    {
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .gpio_num = RMT_LED_STRIP_GPIO_NUM,
        .mem_block_symbols = 64, // increase the block size can make the LED less flickering
        .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
        .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &tx_chan));

    ESP_ERROR_CHECK(rmt_enable(tx_chan));


    for (int led = 0; led < EXAMPLE_LED_NUMBERS; led++) 
    {
        led_strip_pixels[led * 3 + 0] = 0; // GREEN
        led_strip_pixels[led * 3 + 1] = 0; // RED
        led_strip_pixels[led * 3 + 2] = 5; // BLUE
    }

    while (1) 
    {
        ESP_ERROR_CHECK(rmt_transmit(tx_chan, encoder, led_strip_pixels, sizeof(led_strip_pixels), &transmit_cfg));
        ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_chan, portMAX_DELAY));

        vTaskDelay(pdMS_TO_TICKS(EXAMPLE_FRAME_DURATION_MS));
    }
}