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
#include "esp32_rmt.h"
#include "freertos/ringbuf.h"



RingbufHandle_t rb = NULL;

unsigned int rawCode[] = {9024,4512,564,564,564,564,564,564,564,564,564,564,564,564,564,564,564,564,564,1692,564,1692,564,1692,564,1692,564,1692,564,1692,564,564,564,1692,564,1692,564,564,564,564,564,564,564,564,564,564,564,564,564,564,564,564,564,1692,564,1692,564,1692,564,1692,564,1692,564,1692,564,1692,564,40884};
char prontoCode[] = "0000 006C 0022 0002 015B 00AD 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0041 0016 0041 0016 0041 0016 0041 0016 0041 0016 0041 0016 0016 0016 0041 0016 0041 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0041 0016 0041 0016 0041 0016 0041 0016 0041 0016 0041 0016 0041 0016 0622 015B 0057 0016 0E6C";


void ir_fill_item_level(rmt_item32_t* item, int high_us, int low_us)
{
    //   printf("\n\r send RMT_TICK_10_US: %d high_us: %d  low_us: %d", RMT_TICK_10_US, high_us, low_us);

    item->level0 = 1;
    item->duration0 = (high_us/RMT_RX_MULT_FACTOR);
    item->level1 = 0;
    item->duration1 = (low_us/RMT_RX_MULT_FACTOR);
}


void ir_rx_init()
{
    printf("\n %s , ClockDiv:%d", __func__, RMT_CLK_DIV);

    rmt_channel_t rxChannel = (rmt_channel_t)RMT_RX_CHANNEL;
    rmt_config_t rmt_rx;
    rmt_rx.channel = rxChannel;
    rmt_rx.gpio_num = (gpio_num_t) RMT_RX_PIN;
    rmt_rx.clk_div = RMT_CLK_DIV;
    rmt_rx.mem_block_num = 1;
    rmt_rx.rmt_mode = RMT_MODE_RX;
    
    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    rmt_rx.rx_config.idle_threshold = RMT_IDLE_SPACE_TIME;
    rmt_config(&rmt_rx);
    
    
    //get RMT RX ringbuffer
    rmt_driver_install(rxChannel, 1000, 0);
    rmt_get_ringbuf_handler(rxChannel, &rb);
    rmt_rx_start(rxChannel, 1);
    
    printf("\n %s end", __func__);
}




void ir_set_tx_freq(unsigned int hz)
{
    printf("\n %s ", __func__);

    rmt_config_t rmt_tx;
    rmt_tx.channel = (rmt_channel_t)RMT_TX_CHANNEL;
    rmt_tx.gpio_num =(gpio_num_t) RMT_TX_PIN;
    rmt_tx.clk_div = RMT_CLK_DIV;
    rmt_tx.mem_block_num = 1;
    rmt_tx.rmt_mode = RMT_MODE_TX;
    
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_duty_percent = 50;
    rmt_tx.tx_config.carrier_freq_hz = hz;
    rmt_tx.tx_config.carrier_level = (rmt_carrier_level_t)1;
    rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
    rmt_tx.tx_config.idle_level = (rmt_idle_level_t)0;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_config(&rmt_tx);
    rmt_driver_install(rmt_tx.channel, 0, RMT_INTR_NUM);
    
    printf("\n %s end", __func__);
}



void ir_send_raw(const unsigned int buff[], unsigned int len, unsigned int freq)
{
    printf("\n %s ", __func__);

    ir_set_tx_freq(freq*1000);
    
    rmt_channel_t txChannel = (rmt_channel_t) RMT_TX_CHANNEL;
    int nec_tx_num = RMT_TX_DATA_NUM;

    size_t size = (sizeof(rmt_item32_t) * len * nec_tx_num);
    rmt_item32_t* item = (rmt_item32_t*) malloc(size);
    int item_num = len * nec_tx_num;
    memset((void*) item, 0, size);
    
    for(int i=0;i<len/2;i++)
    {
        ir_fill_item_level(item+i,buff[i*2],buff[i*2+1]);
    }

    //To send data according to the waveform items.
    rmt_write_items(txChannel, item, item_num, true);
    //Wait until sending is done.
    rmt_wait_tx_done(txChannel);
    //before we free the data, make sure sending is already done.
    free(item);
    
    rmt_driver_uninstall(txChannel);
    
    printf("\n %s end", __func__);    
}


void ir_send_codes_from_string(char *irCodes)
{
    unsigned int num=0,val=0,numSystem=10,i,index=0;
    unsigned int irTimings[RMT_MAX_IR_CODES];

    printf("\n %s ", __func__);

    for(i=0;irCodes[i];i++)
    {
        val = irCodes[i];
        if((val>='0' && val<='9') || (val>='A' && val<='F'))
        {
            num = num*numSystem + ascii2Hex(val);
        }
        else if(val == ' ')
        {
            irTimings[index] = num;
            if(index == 0  && num == 0)
            {
                numSystem = 16; //Pronto code in hex format
            }
            index++;
            num = 0;
        }
    }
    irTimings[index++] = num;

    if(irTimings[0] == 0)
    {
        ir_send_pronto(irTimings);
    }
    else
    {
        ir_send_raw(irTimings,index,38);
    }
}


void ir_send_pronto(unsigned int prontoCode[])
{   
    printf("\n*%s", __func__);
    
    uint8_t index = 0; 
    
    // Check mode = Oscillated/Learned
    if (prontoCode[index++] != 0x0000)  return ;
    
    uint16_t freq = (int)(1000000 / (prontoCode[index++] * 0.241246));  // Rounding errors will occur, tolerance is +/- 10%
    uint8_t usec = (int)(((1.0 / freq) * 1000000) + 0.5);
    uint8_t once = prontoCode[index++];
    uint8_t rpt = prontoCode[index++];
    
    // ToDo: consider fallback and repeat case to calculate length and skip
    uint8_t length = once * 2;
    //   uint8_t skip = 0;       // repeat = 0 and fallback = 0;
    
    // skip to start of code
    //  index += skip;
    
    for (int i = index;  i < length;  i++) 
    {
        prontoCode[i] *= usec;
    }
    
    ir_send_raw(&prontoCode[index], length, freq);

}



int ir_receive(uint32_t buff[])
{
    printf("\n %s", __func__);

    size_t rx_size = 0;
    int irCodeLength = 0;
    //try to receive data from ringbuffer.
    //RMT driver will push all the data it receives to its ringbuffer.
    //We just need to parse the value and return the spaces of ringbuffer.
    rmt_item32_t* item1 = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, portMAX_DELAY);
    rmt_item32_t* item = item1;
    
    if(item) 
    {
        irCodeLength = rx_size / 2;  
        
        //parse data value from ringbuffer.
        for(int i = 0; i < rx_size / 4 ; i++)
        {
            buff[i*2] = item->duration0*RMT_RX_MULT_FACTOR;
            buff[i*2+1] = item->duration1*RMT_RX_MULT_FACTOR;
            printf( "\n %2d item->duration0: %d \t item->duration1: %d", i,item->duration0*RMT_RX_MULT_FACTOR ,item->duration1*RMT_RX_MULT_FACTOR );
            item++;

        } 
        
        vRingbufferReturnItem(rb, (void*) item1);
    }
    else 
    {
        printf(" Data Not received");
    }    
    
    printf("\n %s end", __func__);
    
    return irCodeLength;
}



void ir_Remote_task(void *param) 
{
    unsigned int irReeciveCodes[RMT_MAX_IR_CODES];
    int len = 0,txCount=0;
    ir_rx_init();

    while(1)
    {
        len = ir_receive(irReeciveCodes);
        if(len>0)
        {
            printf("\n Received Ir Codes");
            for(int i=0;i<len;i++)
            printf("\n [%2d]-%d",i,irReeciveCodes[i]);
        }
        
        txCount++;
        if(txCount == 2)
        {  
            printf("\n Send Raw codes at 38K freq");    
            ir_send_raw(rawCode,68,38);          
        }
        else if(txCount == 4)
        {
            txCount = 0; 
            printf("\n Send Pronto Codes");              
            ir_send_codes_from_string(prontoCode);             
        }
        
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}



