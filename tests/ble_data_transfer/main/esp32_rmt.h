#include <stdint.h>
#include "driver/rmt.h"


#ifndef ESP32_RMT_H
#define ESP32_RMT_H


//#define RMT_TX_CHANNEL    1     /*!< RMT channel for transmitter */
//#define RMT_RX_CHANNEL    0     /*!< RMT channel for receiver */
//#define RMT_RX_GPIO_NUM  19     /*!< GPIO number for receiver */
#define RMT_INTR_NUM     19     /*!< RMT interrupt number, select from soc.h */
#define RMT_CLK_DIV      100    /*!< RMT counter clock divider */
#define RMT_TICK_10_US    (80000000/RMT_CLK_DIV/100000)   /*!< RMT counter  */

#define RMT_TX_DATA_NUM  1    /*!< NEC tx test data number */
#define rmt_item32_tIMEOUT_US  9500   /*!< RMT receiver timeout value(us) */

class ESP32_RMT
{
public:
    ESP32_RMT(); //reserved for future use
    void begin(uint8_t, bool);	

    //transmit functions	
    void rmt_send( unsigned int buf[],  unsigned int len, unsigned int hz);

private: 
    void rmt_tx_init(unsigned int hz);
    void rmt_fill_item_level(rmt_item32_t* , int , int );

    uint8_t txChannel = 1;
    uint8_t rxChannel = 0;
    
    uint8_t txPin =  16; //default transmit pin
    uint8_t rxPin = 19; //default receive pin
    
};



#endif // ESP32_RMT





