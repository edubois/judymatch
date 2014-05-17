#include <unistd.h>
#include "vfwl_globals.h"

#define WAVELET_CODEC_HEADER_SIZE 20

#ifndef VFWL_CODEC_H
#define VFWL_CODEC_H

typedef void (*output_fun)(wav_t *pixels_c1, 
                           wav_t *pixels_c2, 
                           wav_t *pixels_c3, 
                           int h, int w, int final_h, int final_w);

#ifdef __cplusplus
extern "C"
{
#endif

int vfwl_encode_header(unsigned char *header, wavparams_t *p);
int vfwl_decode_header(wavparams_t *p, unsigned char *header);

void vfwl_encode_image(wav_t **out,
                       char *filename, 
                       unsigned char *header,
                       wavparams_t *p);
void vfwl_decode_image(char *filename,
                       wav_t *in, 
                       unsigned char *header);
void vfwl_encode_ch(wav_t **out, 
                    wav_t *channels, 
                    unsigned char* header, 
                    wavparams_t * infos);

void vfwl_decode_ch_h(wav_t **out, 
                      wav_t * in, 
                      unsigned char* header);

ssize_t vfwl_decode_ch(wav_t **out, 
                       wav_t * in, 
                       wavparams_t *p);

void vfwl_quantize(wav_t *mat_in, size_t stop_level, wavparams_t * p);

void fiwt1D_5_3(wav_t *, wav_t *, int, wavparams_t *);
void iiwt1D_5_3(wav_t *, wav_t *, int, wavparams_t *);
void fiwt2D_5_3(wav_t *, wav_t *, wavparams_t *);
void iiwt2D_5_3(wav_t *, wav_t *, wavparams_t *);

void fiwt1D_5_3_slow(wav_t *, wav_t *, int, wavparams_t *);
void iiwt1D_5_3_slow(wav_t *, wav_t *, int, wavparams_t *);
void fiwt2D_5_3_slow(wav_t *, wav_t *, wavparams_t *);
void iiwt2D_5_3_slow(wav_t *, wav_t *, wavparams_t *);

#ifdef __cplusplus
}
#endif

#endif /*VFWL_CODEC_H*/
