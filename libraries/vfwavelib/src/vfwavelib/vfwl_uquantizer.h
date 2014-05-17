#include <unistd.h>
#include "vfwl_codec.h"

#ifndef VFWL_UQUANTIZER_H
#define VFWL_UQUANTIZER_H

typedef struct quantizer_t {
    size_t table_size;
    wav_t *qtable;                  /* Association table between pixel and 
                                       quantified pixel                     */
    int nb_bits;                    /* Number of distinct values            */
} quantizer_t;

void vfwl_uquantizer_init(quantizer_t *q);
void vfwl_uquantize_slow(wav_t *out, wavparams_t *params, quantizer_t *q);

#endif /* VFWL_UQUANTIZER_H */
