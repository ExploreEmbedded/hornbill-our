#ifndef PRONTO_H_
#define  PRONTO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    void extract_pronto_code(uint8_t *data);
    void sendPronto();

#ifdef __cplusplus
}
#endif
#endif