#include <stdio.h>
#include <string.h>

#include "esp32_rmt.h"

#define TRANSMIT 1
#define RECEIVE 0

#if RMT_RX_SELF_TEST
#define RMT_RX_ACTIVE_LEVEL  0   /*!< Data bit is active high for self test mode */
#define RMT_TX_CARRIER_EN    0   /*!< Disable carrier for self test mode  */
#else
//Test with infrared LED, we have to enable carrier for transmitter
//When testing via IR led, the receiver waveform is usually active-low.
#define RMT_RX_ACTIVE_LEVEL  0   /*!< If we connect with a IR receiver, the data is active low */
#define RMT_TX_CARRIER_EN    1   /*!< Enable carrier for IR transmitter test with IR led */
#endif

#define rmt_item32_tIMEOUT_US  9500   /*!< RMT receiver timeout value(us) */

ESP32_RMT::ESP32_RMT()
{
    //printf("booting remote peripheral\n\r");
    //printf("hello world\n\r");
}

void ESP32_RMT::begin(uint8_t pin, bool mode)
{
    if(mode==TRANSMIT)
    {
        txPin = pin;    
    }   
}

void ESP32_RMT::rmt_tx_init(unsigned int hz)
{
    printf("\n\r ESP32_RMT::rmt_tx_init  hz: %d", hz);

    rmt_config_t rmt_tx;
    rmt_tx.channel = (rmt_channel_t)txChannel;
    rmt_tx.gpio_num =(gpio_num_t) txPin;
    rmt_tx.mem_block_num = 1;
    rmt_tx.clk_div = RMT_CLK_DIV;
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_duty_percent = 50;
    rmt_tx.tx_config.carrier_freq_hz = hz;
    rmt_tx.tx_config.carrier_level = (rmt_carrier_level_t)1;
    rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
    rmt_tx.tx_config.idle_level = (rmt_idle_level_t)0;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.rmt_mode = (rmt_mode_t)0;
    rmt_config(&rmt_tx);
    rmt_driver_install(rmt_tx.channel, 0, RMT_INTR_NUM);
    
}


void ESP32_RMT::rmt_fill_item_level(rmt_item32_t* item, int high_us, int low_us)
{
    //   printf("\n\r ESP32_RMT::send RMT_TICK_10_US: %d high_us: %d  low_us: %d", RMT_TICK_10_US, high_us, low_us);

    item->level0 = 1;
    item->duration0 = (high_us) / 10 * RMT_TICK_10_US;
    item->level1 = 0;
    item->duration1 = (low_us) / 10 * RMT_TICK_10_US;
}




void ESP32_RMT::rmt_send(unsigned int buff[], unsigned int len, unsigned int hz)
{
    printf("\n ESP32_RMT::rmt_send");
    rmt_tx_init(hz);

    //printf("cmd: %d, data: %d \n\r ", cmd, addr);
    int channel = txChannel;
    int nec_tx_num = RMT_TX_DATA_NUM;

    size_t size = (sizeof(rmt_item32_t) * len * nec_tx_num);
    //each item represent a cycle of waveform.
    rmt_item32_t* item = (rmt_item32_t*) malloc(size);
    int item_num = len * nec_tx_num;
    memset((void*) item, 0, size);
    
    for(int i=0;i<len/2;i++)
    {
        rmt_fill_item_level(item+i,buff[i*2],buff[i*2+1]);
    }

    //To send data according to the waveform items.
    rmt_write_items((rmt_channel_t)channel, item, item_num, true);
    //Wait until sending is done.
    rmt_wait_tx_done((rmt_channel_t)channel);
    //before we free the data, make sure sending is already done.
    free(item);
    
}




