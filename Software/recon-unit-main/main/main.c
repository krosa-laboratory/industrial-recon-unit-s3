#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

static const char *TAG = "SYS_BOOT";

void app_main(void)
{
    // --- 1. Información del Chip ---
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    
    ESP_LOGI(TAG, "------------------------------------------------");
    ESP_LOGI(TAG, "Iniciando Industrial Cyber-Recon Unit...");
    ESP_LOGI(TAG, "------------------------------------------------");
    
    ESP_LOGI(TAG, "CPU: ESP32-S3 | Cores: %d | Rev: %d", chip_info.cores, chip_info.revision);
    
    // --- 2. Validación de Flash Interna ---
    if(esp_flash_get_size(NULL, &flash_size) == ESP_OK) {
        ESP_LOGI(TAG, "Flash Interna: %" PRIu32 " MB (Debería ser 16MB)", flash_size / (1024 * 1024));
    } else {
        ESP_LOGE(TAG, "ERROR CRÍTICO: No se puede leer el tamaño de la Flash");
    }

    // --- 3. Validación de PSRAM (SPIRAM) ---
    size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    
    if (free_psram > 0) {
        ESP_LOGI(TAG, "PSRAM Detectada: %.2f MB disponibles", (float)free_psram / (1024 * 1024));
    } else {
        ESP_LOGE(TAG, "FALLO PSRAM: Detectados 0 Bytes. Revisa sdkconfig -> Component config -> ESP32S3-Specific -> SPIRAM");
        return; // Abortar si no hay RAM
    }

    // --- 4. Prueba de Estrés de Memoria ---
    ESP_LOGI(TAG, "Ejecutando Test de Asignación en PSRAM...");
    
    // Intentamos reservar un bloque masivo de 4MB (Simulando el Buffer del Sniffer)
    size_t test_size = 4 * 1024 * 1024; 
    uint8_t *big_buffer = (uint8_t *)heap_caps_malloc(test_size, MALLOC_CAP_SPIRAM);

    if (big_buffer != NULL) {
        ESP_LOGI(TAG, "Test OK: Bloque de 4MB reservado correctamente en %p", big_buffer);
        
        // Prueba de escritura/lectura rápida para verificar integridad
        big_buffer[0] = 0xAA;
        big_buffer[test_size - 1] = 0x55;
        
        if (big_buffer[0] == 0xAA && big_buffer[test_size - 1] == 0x55) {
             ESP_LOGI(TAG, "Integridad de Memoria: VERIFICADA");
        } else {
             ESP_LOGE(TAG, "Integridad de Memoria: FALLÓ (Corrupción de datos)");
        }
        
        heap_caps_free(big_buffer);
        ESP_LOGI(TAG, "Memoria liberada. Sistema listo para Fase 2.");
        
    } else {
        ESP_LOGE(TAG, "Test FALLIDO: No se pudieron reservar 4MB continuos.");
    }
}