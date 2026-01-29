/**
 * 
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"      // Low level SPI controller library
#include "esp_lcd_panel_io.h"       // Data sender for the Display
#include "esp_lcd_panel_vendor.h"   // Manufacturers drivers for Display
#include "esp_lcd_panel_ops.h"      // Generic operations for Display

// GPIO DEFINITION
#define PIN_LCD_SCLK 12
#define PIN_LCD_MOSI 11
#define PIN_LCD_DC   6
#define PIN_LCD_CS   10
#define PIN_LCD_RST  14
#define PIN_LCD_BL   38

// Display params
#define LCD_H_RES 128
#define LCD_V_RES 128
#define LCD_PIXEL_CLOCK_HZ 20000000 // 20 MHz

void app_main(void)
{

    // --- SPI Bus initialization
    spi_bus_config_t spi_config = {
        .sclk_io_num = PIN_LCD_SCLK,
        .mosi_io_num = PIN_LCD_MOSI,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        // This is the maximum data block to send with DMA
        // Defined as: (TOTAL PIXELS * 2 bytes/pixel) + security margin (ie. 8)
        .max_transfer_sz = LCD_H_RES * LCD_V_RES * 2 + 8
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &spi_config, SPI_DMA_CH_AUTO));

    // --- Data sender initialization
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_LCD_DC,
        .cs_gpio_num = PIN_LCD_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI2_HOST, &io_config, &io_handle));

    // --- Intall Display driver
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_LCD_RST,
        .rgb_endian = LCD_RGB_ENDIAN_RGB,
        .bits_per_pixel = 16
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // --- Power-On Sequence
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    // --- Display Test
    printf("Painting display full red...\n");

    // Get memory for 10 lines for Display.
    // MALLOC_CAP_DMA forces memory to be in internal SRAM so DMA can access it.
    uint16_t *buffer = heap_caps_malloc(LCD_H_RES * 10 * sizeof(uint16_t), MALLOC_CAP_DMA);

    // Fill the buffer with RED
    for (int i = 0; i < LCD_H_RES * 10; i++) buffer[i] = 0xF800;

    // Send the buffer in a loop to fill the screen
    for (int i = 0; i < LCD_V_RES; i +=10)
        esp_lcd_panel_draw_bitmap(panel_handle, 0, i, LCD_H_RES, i + 10, buffer);

    printf("Display ready!");

    // Free memory
    heap_caps_free(buffer);

}
