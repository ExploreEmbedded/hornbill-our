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

void ir_fill_item_level(rmt_item32_t* item, int high_us, int low_us)
{
    #if RMT_DEBUG_ENABLE
    printf("\nhigh_us:%d  low_us:%d", high_us, low_us);
    #endif

    item->level0 = 1;
    item->duration0 = (high_us/RMT_RX_MULT_FACTOR);
    item->level1 = 0;
    item->duration1 = (low_us/RMT_RX_MULT_FACTOR);
}


void ir_rx_init()
{
    #if RMT_DEBUG_ENABLE
    printf("\n %s , ClockDiv:%d", __func__, RMT_CLK_DIV);
    #endif
    
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
}

void ir_tx_init(unsigned int hz)
{
    #if RMT_DEBUG_ENABLE
    printf("\n %s ", __func__);
    #endif
    
    ir_set_tx_freq(hz);
    rmt_driver_install((rmt_channel_t)RMT_TX_CHANNEL, 1000, RMT_INTR_NUM);

}


void ir_set_tx_freq(unsigned int hz)
{
    #if RMT_DEBUG_ENABLE
    printf("\n %s hz: %d", __func__, hz);
    #endif
    
    rmt_config_t rmt_tx;
    rmt_tx.channel = (rmt_channel_t)RMT_TX_CHANNEL;
    rmt_tx.gpio_num =(gpio_num_t) RMT_TX_PIN;
    rmt_tx.clk_div = RMT_CLK_DIV;
    rmt_tx.mem_block_num = 1;
    rmt_tx.rmt_mode = RMT_MODE_TX;
    
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_duty_percent = 50;
    rmt_tx.tx_config.carrier_freq_hz = hz;
    rmt_tx.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
    rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    rmt_tx.tx_config.idle_output_en = true;
    
    rmt_config(&rmt_tx);    
}

//ToDo: MARK and SPACE generated with this are sometimes beyond tolerance limit
// tolerance is +/- 10% of carrier freq.

void ir_send_raw(const unsigned int buff[], unsigned int len, unsigned int freq)
{
    #if RMT_DEBUG_ENABLE
    printf("\n %s ", __func__);
    #endif
    
    ir_set_tx_freq(freq);
    
    rmt_channel_t txChannel = (rmt_channel_t) RMT_TX_CHANNEL;
    int nec_tx_num = RMT_TX_DATA_NUM;

    int item_num = len * nec_tx_num;
    size_t size = (sizeof(rmt_item32_t) * item_num);
    
    rmt_item32_t* item = (rmt_item32_t*) malloc(size);
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
}


void ir_send_codes_from_string(char *irCodes)
{
    unsigned int num=0,val=0,numSystem=10,i,index=0;
    unsigned int irTimings[RMT_MAX_IR_CODES];

    #if RMT_DEBUG_ENABLE
    printf("\n %s ", __func__);
    #endif
    
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
    #if RMT_DEBUG_ENABLE
    printf("\n*%s", __func__);
    #endif
    
    // Check mode = Oscillated/Learned
    if (prontoCode[0] != 0x0000)  return ;
    
    uint16_t freq = (int)(1000000 / (prontoCode[1] * 0.241246));  // Rounding errors will occur, tolerance is +/- 10%
    uint8_t usec = (int)((1000000.0/(float)freq) + 0.5);
    uint8_t once = prontoCode[2];
    uint8_t rpt = prontoCode[3];
    
    // ToDo: consider fallback and repeat case to calculate length and skip
    uint8_t length = once ? (once * 2) : (rpt * 2);
    //   uint8_t skip = 0;       // repeat = 0 and fallback = 0;
    
    // skip to start of code
    //  index += skip;
    
    printf("freq: %d", freq);
    
    for (int i = 4;  i < length+4;  i++) 
    {
        prontoCode[i] *= usec;
    }
    
    ir_send_raw(&prontoCode[4], length, freq);
}



int ir_receive(uint32_t buff[])
{
    #if RMT_DEBUG_ENABLE
    printf("\n %s", __func__);
    #endif
    
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

            #if RMT_DEBUG_ENABLE
            printf( "\n %2d item->duration0: %d \t item->duration1: %d", i,item->duration0*RMT_RX_MULT_FACTOR ,item->duration1*RMT_RX_MULT_FACTOR );
            #endif  
            
            item++;
        } 
        
        vRingbufferReturnItem(rb, (void*) item1);
    }   
    
    return irCodeLength;
}






