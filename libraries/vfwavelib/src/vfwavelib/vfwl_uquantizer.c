/**
 * UNIFORM QUANTIZER
 *
 * Very Fast Wavelet encoding/decoding library V.01 alpha
 * Copyright (C) 2008 Eloi Du Bois
 * eloi.du.bois[AT]gmail.com.toremove
 *
 * Bibliography: Image and Video Compression for Multimedia Engineering.
 *               Yun Q. Shi Huifang Sun.
 *
 * PLEASE PUT MY NAME IN YOUR SPECIAL THANKS IF YOU USE THIS LIB
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for  more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vfwl_uquantizer.h"

void
vfwl_uquantizer_init(quantizer_t *q) {
    size_t k,l;
    int step_size;
    /* force nb_bits to be lesser than 8 */
    if (q->nb_bits > 8)
        q->nb_bits = 8;
    if (q->nb_bits > 64) {
        fprintf(stderr, "%s: Quantizer number of bits to high (%d)\n", 
                __FUNCTION__, q->nb_bits);
        return;
    }
    step_size = (int)q->table_size / pow(2.0, q->nb_bits);
    printf("Step size: %d\n", step_size);
    q->qtable = (wav_t*)malloc(q->table_size*sizeof(wav_t));
    /* Init association table between pixel value and quantized value */
    for(k = 0; k < q->table_size; k+=l) {
        for(l = 0; l < step_size; l++) {
            if ((k+l) >= q->table_size)
                break;
            q->qtable[k+l] = k;
        }
    }
}

void
vfwl_uquantize_slow(wav_t *out, wavparams_t *params, quantizer_t *q) {
    wav_t *qtable = q->qtable;
    size_t size = params->w * params->h * params->nb_channels;
    size_t k;
    size_t ts = q->table_size;
    wav_t *tmp = out;
    for(k = 0; k < size; k++) {
        *tmp = qtable[*tmp % ts];
        tmp++;
    }
}

void
vfwl_uquantizer_free(quantizer_t *q) {
    free(q->qtable);
}
