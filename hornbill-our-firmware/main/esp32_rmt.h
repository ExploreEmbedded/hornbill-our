#ifndef ESP32_RMT_H
#define ESP32_RMT_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_reg.h"
#include "esp_err.h"
#include "esp_log.h"

#define RMT_DEBUG_ENABLE   0

#define RMT_MAX_IR_CODES   150

#define RMT_CLOCK          (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ*1000000) // 80Mhz
#define RMT_CLK_DIV        200     /*!< RMT counter clock divider */
#define RMT_TICK_10_US    (RMT_CLOCK/RMT_CLK_DIV/100000)   /*!< RMT counter value for 10 us.(Source clock is APB clock) */
#define RMT_RX_MULT_FACTOR (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ/80)
#define RMT_INTR_NUM       19     /*!< RMT interrupt number, select from soc.h */
#define RMT_RX_CHANNEL      0
#define RMT_RX_PIN         19 //default receive pin

#define RMT_IDLE_SPACE_TIME    65000    /*!< RMT receiver timeout value(us) range 0-65536us */
#define RMT_TX_CHANNEL             1
#define RMT_TX_PIN                16 //default transmit pin
#define RMT_TX_CARRIER_EN          1

#define ascii2Hex(Asc)  (((Asc)>0x39) ? ((Asc) - 0x37): ((Asc) - 0x30))

void ir_fill_item_level(rmt_item32_t* item, int high_us, int low_us);
void ir_rx_init();
void ir_tx_init(unsigned int hz);
void ir_set_tx_freq(unsigned int hz);
void ir_send_raw(const unsigned int buff[], unsigned int len, unsigned int freq);
void ir_send_codes_from_string(char *irCodes);
void ir_send_pronto(unsigned int prontoCode[]);
int ir_receive(uint32_t buff[]);

#endif // ESP32_RMT





