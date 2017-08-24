#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h" //data logging
#include "nvs_flash.h" //permnant

//hornbill our files
#include "ble.h"
extern bool receiveFlag;
extern unsigned int code[RMT_MAX_IR_CODES];



void app_main()
{
    esp_err_t ret;

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s initialize controller failed\n", __func__);
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s enable controller failed\n", __func__);
        return;
    }
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s init bluetooth failed\n", __func__);
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s enable bluetooth failed\n", __func__);
        return;
    }

    esp_ble_gatts_register_callback(gatts_event_handler);
    esp_ble_gap_register_callback(gap_event_handler);
    esp_ble_gatts_app_register(PROFILE_A_APP_ID);
    
    receiveFlag = 0;

#ifdef CONFIG_MBEDTLS_DEBUG
    const size_t stack_size = 36*1024;
#else
    const size_t stack_size = 36*1024;
#endif

    xTaskCreatePinnedToCore(&ir_Remote_task, "ir_Remote_task", stack_size, NULL, 4, NULL, 1);

}   
