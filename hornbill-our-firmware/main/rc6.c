#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_reg.h"


#define RMT_RX_ACTIVE_LEVEL  0   /*!< If we conrct with a IR receiver, the data is active low */
#define RMT_TX_CARRIER_EN    1   /*!< Enable carrier for IR transmitter test with IR led */

#define RMT_TX_CHANNEL    1     /*!< RMT channel for transmitter */
#define RMT_TX_GPIO_NUM  16     /*!< GPIO number for transmitter signal */
#define RMT_RX_CHANNEL    0     /*!< RMT channel for receiver */
#define RMT_RX_GPIO_NUM  19     /*!< GPIO number for receiver */
#define RMT_CLK_DIV      100    /*!< RMT counter clock divider */
#define RMT_TICK_10_US    (80000000/RMT_CLK_DIV/100000)   /*!< RMT counter value for 10 us.(Source clock is APB clock) */


#define MIN_RC6_SAMPLES      1
#define RC6_HDR_MARK      2666
#define RC6_HDR_SPACE      889
#define RC6_T1             444
#define RC6_RPT_LENGTH   46000

#define rmt_item32_tIMEOUT_US  9500   /*!< RMT receiver timeout value(us) */


/*
 * @brief Build register value of waveform for rc one data bit
 */
inline void rc_fill_item_level(rmt_item32_t* item, int high_us, int low_us)
{
    item->level0 = 1;
    item->duration0 = (high_us) / 10 * RMT_TICK_10_US;
    item->level1 = 0;
    item->duration1 = (low_us) / 10 * RMT_TICK_10_US;
}

/*
 * @brief Generate rc header value: active 9ms + negative 4.5ms
 */
static void rc_fill_item_header(rmt_item32_t* item)
{
    rc_fill_item_level(item, RC6_HDR_MARK, RC6_HDR_SPACE);
}

/*
 * @brief Generate rc data bit 1: positive 0.56ms + negative 1.69ms
 */
static void rc_fill_item_bit_one(rmt_item32_t* item, uint16_t dur)
{
    printf("\n\r duration %d", dur);
    item->level0 = 1;
    item->duration0 = (RC6_T1) / 10 * RMT_TICK_10_US;
    item->level1 = 0;
    item->duration1 = (RC6_T1) / 10 * RMT_TICK_10_US;
}

/*
 * @brief Generate rc data bit 0: positive 0.56ms + negative 0.56ms
 */
static void rc_fill_item_bit_zero(rmt_item32_t* item, uint16_t dur)
{
    item->level0 = 0;
    item->duration0 = (dur) / 10 * RMT_TICK_10_US;
    item->level1 = 1;
    item->duration1 = (dur) / 10 * RMT_TICK_10_US;
}

void rmt_tx_init()
{
    rmt_config_t rmt_tx;
    rmt_tx.channel = RMT_TX_CHANNEL;
    rmt_tx.gpio_num = RMT_TX_GPIO_NUM;
    rmt_tx.mem_block_num = 1;
    rmt_tx.clk_div = RMT_CLK_DIV;
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_duty_percent = 50;
    rmt_tx.tx_config.carrier_freq_hz = 38000;
    rmt_tx.tx_config.carrier_level = 1;
    rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
    rmt_tx.tx_config.idle_level = 0;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.rmt_mode = 0;
    rmt_config(&rmt_tx);
    rmt_driver_install(rmt_tx.channel, 0, 0);
}


void rmt_rx_init()
{

    rmt_config_t rmt_rx;
    rmt_rx.channel = RMT_RX_CHANNEL;
    rmt_rx.gpio_num = RMT_RX_GPIO_NUM;
    rmt_rx.clk_div = RMT_CLK_DIV;
    rmt_rx.mem_block_num = 1;
    rmt_rx.rmt_mode = RMT_MODE_RX;
    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    rmt_rx.rx_config.idle_threshold = rmt_item32_tIMEOUT_US / 10 * (RMT_TICK_10_US);
    rmt_config(&rmt_rx);
    rmt_driver_install(rmt_rx.channel, 1000, 0);
}

static void changeTxConfig()
{
  rmt_config_t rmt_tx;
  rmt_tx.channel = RMT_TX_CHANNEL;
  rmt_tx.gpio_num = RMT_TX_GPIO_NUM;
  rmt_tx.mem_block_num = 1;
  rmt_tx.clk_div = RMT_CLK_DIV;
  rmt_tx.tx_config.loop_en = false;
  rmt_tx.tx_config.carrier_duty_percent = 50;
  rmt_tx.tx_config.carrier_freq_hz = 36000;
  rmt_tx.tx_config.carrier_level = 1;
  rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
  rmt_tx.tx_config.idle_level = 0;
  rmt_tx.tx_config.idle_output_en = true;
  rmt_tx.rmt_mode = 0;
  rmt_config(&rmt_tx);
}

static void rc6_build_items(rmt_item32_t* item, unsigned long data,  int nbits)
{
    printf("\n\r first header sent");

    //header
    rc_fill_item_header(item++);
    //first bit
    rc_fill_item_bit_one(item++, RC6_T1);

    // Data
    for (unsigned long  i = 1, mask = 1UL << (nbits - 1);  mask;  i++, mask >>= 1)
    {
      // The fourth bit we send is a "double width trailer bit"
      int  t = (i == 4) ? (RC6_T1 * 2) : (RC6_T1) ;
      if (data & mask) {
        rc_fill_item_bit_one(item, t);
      } else {
        rc_fill_item_bit_zero(item, t);
      }
      item++;
    }

}

void  sendRC6 (unsigned long data,  int nbits)
{

  changeTxConfig();

  size_t size = (sizeof(rmt_item32_t) * 26 * 1);
  //each item represent a cycle of waveform.
  printf("allocatig memory \n");

  rmt_item32_t* item = (rmt_item32_t*) malloc(size);
  memset((void*) item, 0, size);

  rc6_build_items(item, data, nbits );
  //rc_fill_item_header(item);
  //printf("\n\r first header sent");
  //  item++;

	// Start bit
	//rc_fill_item_header(item);
  //printf("\n\r second header sent");
  /*
  item++;

	// Data
	for (unsigned long  i = 1, mask = 1UL << (nbits - 1);  mask;  i++, mask >>= 1)
  {
		// The fourth bit we send is a "double width trailer bit"
		int  t = (i == 4) ? (RC6_T1 * 2) : (RC6_T1) ;
		if (data & mask) {
			rc_fill_item_bit_one(item, t);
		} else {
			rc_fill_item_bit_zero(item, t);
		}
    item++;
	}

	//ToDo Always end with the LED off
  //To send data according to the waveform items.
  printf("writing data \n, %d", sizeof(item));
  */
  rmt_write_items(RMT_TX_CHANNEL, item, 26, true);
  //Wait until sending is done.
  rmt_wait_tx_done(RMT_TX_CHANNEL);
  //before we free the data, make sure sending is already done.
  free(item);

}
