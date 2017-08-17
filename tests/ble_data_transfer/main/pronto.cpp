#include "esp32_rmt.h"
#include "esp_log.h" //data logging
#include "pronto.h"

#define PACKET_SIZE  18
#define FLAG_INDEX  0
#define LENGTH_INDEX  1
#define MAX_PRONTO_CODE_LENGTH  100

unsigned int prontoCode[MAX_PRONTO_CODE_LENGTH];
ESP32_RMT esp32_rmt;

void extract_pronto_code(uint8_t *data)
{
    printf("\n%s", __func__);

    bool flag = data[FLAG_INDEX];
    uint8_t len = data[LENGTH_INDEX];
    static uint8_t index = 0;
    
    if (!flag){
        // reset for new code
        index = 0;
    }
    
    for(int i = LENGTH_INDEX+1; i < PACKET_SIZE; i += 2){
        prontoCode[index++] = (data [i]<< 8) | data [i+1];
        printf("\n i:%2d prontoCode[%d]: 0x%x", i, index-1, prontoCode[index-1]);
        
        if(index == len){
            // all packets are received, generate waveform
            sendPronto();
            break;
        }
    }
    
}

void sendPronto()
{   
    printf("\n%s", __func__);
    
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
    
    esp32_rmt.rmt_send(&prontoCode[index], length, freq);

}