#include <unistd.h>

#ifndef VFWL_GLOBALS_H
#define VFWL_GLOBALS_H

//#define WAVT_16

#define INT_SIZE            8

#ifdef WAVT_16
   #define WAVT_BYTE_SIZE   2
   #define WB_PER_128b      8
#else
   #define WAVT_BYTE_SIZE   2
   #define WB_PER_128b      2
#endif


typedef struct {
    size_t w, h;
    unsigned long int nb_levels;
    float q_max;
    float quantization_step;
    unsigned long int nb_channels;
    unsigned long int bpp;
    unsigned long int current_level;
    /* Frequencies of symbols, used for compression */
    unsigned long int *symb_counter;
} wavparams_t;

#ifdef WAVT_16
   typedef short wav_t;
#else
   typedef long long wav_t;
#endif

#endif  /* VFWL_GLOBALS_H */
