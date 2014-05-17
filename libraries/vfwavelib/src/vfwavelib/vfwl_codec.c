/* Very Fast Wavelet encoding/decoding library V.01 alpha
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
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <emmintrin.h>

#include "vfwl_codec.h"
#include "vfwl_image.h"
#include "vfwl_uquantizer.h"

/**
*   \brief  This function encode header.
*   \param	header  The destination header buffer.
*   \param	p       The wavelet codec parameters structure.
*   \return	1 if fails, 0 otherwise.
*/
int
vfwl_encode_header(unsigned char *header, wavparams_t *p) {
	/* Check if image's dimensions are power of 2. */
	if (ceil(log(p->h)/log(2)) != (int)(log(p->h)/log(2)) ||
        ceil(log(p->w)/log(2)) != (int)(log(p->w)/log(2))) {
		fprintf(stderr, "%s: The image is not of a size power of 2\n", __FUNCTION__);
		return 1;
	}

	*(unsigned int *)header = p->nb_channels;
	*(unsigned int *)(header+INT_SIZE) = p->nb_levels;
	*(unsigned int *)(header+INT_SIZE*2) = p->h;
	*(unsigned int *)(header+INT_SIZE*3) = p->w;
	*(unsigned int *)(header+INT_SIZE*4) = p->bpp;
	return 0;
}

void
vfwl_encode_image(wav_t **out,
                  char *filename, 
                  unsigned char *header,
                  wavparams_t *p) {
    image_t img;
    quantizer_t q;
    vfwl_init_image(&img);
    vfwl_import_pnm(&img, filename);
    p->w = img.w;
    p->h = img.h;
    p->nb_channels = img.nb_channels;
    /* If RGB, then convert into Y, Blue chrominancy, Red chrominancy. */
/*    if (img.nb_channels == 3)
        vfwl_convert_RGB_to_YCbCr(&img);
*/
    vfwl_encode_ch(out, (wav_t*)img.channels, header, p);
    q.nb_bits = 16;
    q.table_size = 65536;
    vfwl_uquantizer_init(&q);
    vfwl_uquantize_slow(*out, p, &q);
    vfwl_uquantizer_free(&q);
}

/**
 * \brief Encode channels
 * \param out output
 * \param channels  channels buffer   (will be modified ! 
 *                                     Out: Contains the step before last 
 *                                     level decomposition)
 * \param header header
 * \param p Codec parameters
 */
void
vfwl_encode_ch(wav_t **out, 
               wav_t * channels, 
               unsigned char* header, 
               wavparams_t * p) {
    int k;
    size_t img_size = p->w * p->h;
    if (!vfwl_encode_header(header, p)) {
        *out = _mm_malloc(sizeof(wav_t*) * img_size * p->nb_channels, 16);
        for(k = 0; k < p->nb_channels; k++) {
            fiwt2D_5_3_slow(&((*out)[k*img_size]), &channels[k*img_size], p);
        }
    }
}

/**
*   \brief    This function decode header.
*   \param	p       The wavelet codec parameters structure.
*   \param	header  The source header buffer.
*   \return	1 if fails, 0 otherwise.
*/
int
vfwl_decode_header(wavparams_t *p, unsigned char *header) {
	p->nb_channels = *(unsigned int*)header;
	p->nb_levels = *(unsigned int*)(header+INT_SIZE);
	p->h = *(unsigned int*)(header+INT_SIZE*2);
	p->w = *(unsigned int*)(header+INT_SIZE*3);
	p->bpp = *(unsigned int*)(header+INT_SIZE*4);

    /* Check if image's dimensions are power of 2. */
	if (ceil(log(p->h)/log(2)) != (int)(log(p->h)/log(2)) || 
        ceil(log(p->w)/log(2)) != (int)(log(p->w)/log(2))) {
		fprintf(stderr, "%s: The image is not of a size power of 2\n", __FUNCTION__);
		return 1;
	}

    return 0;
}

void
vfwl_decode_image(char *filename,
                  wav_t *in, 
                  unsigned char *header) {
    image_t img;
    wav_t *reco = NULL;
    wavparams_t params;

    vfwl_decode_header(&params, header);
    vfwl_decode_ch(&reco, in, &params);

    img.channels = (pix_t*)reco;
    img.w = params.w;
    img.h = params.h;
    img.nb_channels = params.nb_channels;

    /* If necessary, revert to Red Green Blue. */
/*    if (img.nb_channels == 3)
        vfwl_convert_YCbCr_to_RGB(&img);*/
    vfwl_export_pnm(filename, &img, 1);
    _mm_free(reco);
}

/**
 * \brief Encode channels
 * \param out output
 * \param channels  channels buffer   (will be modified ! 
 *                                     Out: Contains the step before last 
 *                                     level decomposition)
 * \param header header
 * \param p Codec parameters
 */
void
vfwl_decode_ch_h(wav_t **out, 
                 wav_t * in, 
                 unsigned char* header) {
    wavparams_t p;
    if (!vfwl_decode_header(&p, header)) {
        vfwl_decode_ch(out, in, &p);
    }
}

ssize_t
vfwl_decode_ch(wav_t **out, 
               wav_t * in, 
               wavparams_t *p) {
    int k;
    size_t img_size = p->w * p->h;
    *out = _mm_malloc(sizeof(wav_t*) * img_size * p->nb_channels, 16);
    for(k = 0; k < p->nb_channels; k++) {
        iiwt2D_5_3_slow(&((*out)[k*img_size]), &in[k*img_size], p);
    }
    return 0;
}

/**
* \brief Quantize the wavelets coefficients of the image exept the very low
*        frequencies.
*/
void
vfwl_quantize(wav_t *mat_in, size_t stop_level, wavparams_t * p) {
    size_t size = p->w * p->h * p->nb_channels;
    size_t x, y, k, width = p->w;
    size_t w_start = p->w / pow(2.0f, p->nb_levels - stop_level);
    size_t h_stop = p->h / pow(2.0f, p->nb_levels - stop_level);
    float q = p->q_max;
    wav_t *tmp = NULL;
    for(y = 0; y < h_stop; y++) {
        tmp = &mat_in[y*width + w_start];
        for(x = w_start; x < p->w; x++) {
            *tmp = (wav_t)(((wav_t)(*tmp / q))*q);
            tmp++;
        }
    }
    for(k = 0; k < size; k++) {
        *tmp = (wav_t)(((wav_t)(*tmp / q))*q);
        tmp++;
    }
}

/**
 * \brief Forward IWT (5,3) in 2-Dimensions
 * \param out output
 * \param in input   (will be modified ! Out: Contains the step before last 
 *                    level decomposition)
 * \param n input length
 * \param p Codec parameters
 */
void
fiwt2D_5_3(wav_t *mat_out, wav_t *mat_in, wavparams_t * p) {
    ssize_t i, x, y, k, h, l = 0;
    ssize_t nn, ns2;
    size_t width = p->w;
    size_t height = p->h;
    wav_t *in = NULL;
    wav_t *out = NULL;
    __m128i in0, in1, in2, itmp1, itmp2, itmp3, itmp4;
#ifdef WAVT_16
    __m128i ones = _mm_set1_epi16(1);
    __m128i twos = _mm_set1_epi16(2);
#else
    __m128i ones = _mm_set1_epi32(1);
    __m128i twos = _mm_set1_epi32(2);
#endif /* WAVT_16 */
    __m128i *v_inm2, *v_inm1, *v_in, *v_inp2;
    __m128i *v_outh, *v_outhm1, *v_outl;

    /* Do a n_level cut iteration */
    nn = width;
    ns2 = width;
    for (k = p->nb_levels - 1; k >= 0; k--) {
        /******************************** COLUMNS ***************************/
        for(y = 0; y < height; y++) {
            /* Can't find a better optimization... */
            out = &mat_out[y*width];
            in  = &mat_in[y*width];
            /* Compute new length / 2 */
            ns2 = nn / 2;

            /* Note: splitting is done while predicting & updating */
            i = 2;
            /* High & low frequencies offsets */
            l = 0;
            h = ns2;
            /* Primary prediction */
            out[h] = in[i - 1] - ((wav_t)(in[i - 2] + in[i] + 1) >> 1);
            /* Primary update */
            out[l] = in[i - 2];
            i+=2;
            l++;
            for(h = ns2 + 1; h < ns2 + 8; h++,l++,i+=2) {
                /* Predictions */
                out[h] = in[i - 1] - ((wav_t)(in[i - 2] + in[i] + 1) >> 1);
                /* Updates */
                out[l] = in[i - 2] + ((wav_t)(out[h - 1] + out[h] + 2) >> 2);
            }
            v_inm2 = (__m128i*)(in + 2*8);
            v_inp2 = (__m128i*)(in + WB_PER_128b + 2*8);
            /* Predictions & updates */
            for (h = ns2 + 8; h < nn - WB_PER_128b - 1; h+=WB_PER_128b, 
                                                      l+=WB_PER_128b, 
                                                      i+=2*WB_PER_128b) {
                itmp1 = _mm_load_si128(v_inm2);
                itmp2 = _mm_load_si128(v_inp2);
                /* Predictions with SIMD power :) */
                /* out[h] = in[i - 1] - ((in[i - 2] + in[i] + 1) / 2);*/
#ifdef WAVT_16
                /* 1 2 3 4 5 6 7 8 */
                itmp3 = _mm_shufflelo_epi16(itmp1, _MM_SHUFFLE(3, 1, 2, 0));
                /* 1 3 2 4 5 7 6 8 */
                itmp3 = _mm_shufflehi_epi16(itmp3, _MM_SHUFFLE(3, 1, 2, 0));
                /* 1 3 5 7 2 4 6 8 */
                itmp3 = _mm_shuffle_epi32(itmp3, _MM_SHUFFLE(3, 1, 2, 0));
                /* Idem */
                itmp4 = _mm_shufflelo_epi16(itmp2, _MM_SHUFFLE(3, 1, 2, 0));
                itmp4 = _mm_shufflehi_epi16(itmp4, _MM_SHUFFLE(3, 1, 2, 0));
                itmp4 = _mm_shuffle_epi32(itmp4, _MM_SHUFFLE(3, 1, 2, 0));
                in2 = _mm_unpacklo_epi64(itmp3, itmp4);
                in1 = _mm_unpackhi_epi64(itmp3, itmp4);

                itmp1 = _mm_srli_si128 (in2, WAVT_BYTE_SIZE);
                in0 = _mm_insert_epi16 (itmp1, in[i + 2*WB_PER_128b - 2], 7);
                itmp1 = _mm_add_epi16(in2,  in0);
                itmp1 = _mm_add_epi16(itmp1, ones);
                itmp1 = _mm_srai_epi16(itmp1, 1);
                itmp1 = _mm_sub_epi16(in1, itmp1);
                _mm_store_si128((__m128i*)(out + h), itmp1);

                /* Updates */
                /* out[l] = in[i - 2] + ((out[h - 1] + out[h] + 2) / 4);*/
                itmp3 = _mm_loadu_si128((__m128i*)(out + h - 1));
                itmp2 = _mm_add_epi16(itmp3, itmp1);
                itmp2 = _mm_add_epi16(itmp2, twos);
                itmp2 = _mm_srai_epi16(itmp2, 2);
                itmp2 = _mm_add_epi16(in2, itmp2);
#else
                itmp3 = _mm_shuffle_epi32(itmp1, _MM_SHUFFLE(3, 1, 2, 0));
                itmp4 = _mm_shuffle_epi32(itmp2, _MM_SHUFFLE(3, 1, 2, 0));
                in2 = _mm_unpacklo_epi64(itmp3, itmp4);
                in1 = _mm_unpackhi_epi64(itmp3, itmp4);

                itmp1 = _mm_srli_si128 (in2, WAVT_BYTE_SIZE);
                itmp1 = _mm_insert_epi16 (itmp1, 
                                          in[i + 2*WB_PER_128b - 2], 6);
                in0 = _mm_insert_epi16 (itmp1, 
                                        in[i + 2*WB_PER_128b - 2]>>16, 7);

                itmp1 = _mm_add_epi32(in2,  in0);
                itmp1 = _mm_add_epi32(itmp1, ones);
                itmp1 = _mm_srai_epi32(itmp1, 1);
                itmp1 = _mm_sub_epi32(in1, itmp1);
                _mm_store_si128((__m128i*)(out + h), itmp1);

                /* Updates */
                /* out[l] = in[i - 2] + ((out[h - 1] + out[h] + 2) / 4);*/
                itmp3 = _mm_loadu_si128((__m128i*)(out + h - 1));
                itmp2 = _mm_add_epi32(itmp3, 
                                   itmp1);
                itmp2 = _mm_add_epi32(itmp2, twos);
                itmp2 = _mm_srai_epi32(itmp2, 2);
                itmp2 = _mm_add_epi32(in2, itmp2);
#endif   /* WAVT_16 */
                _mm_store_si128((__m128i*)(out +  l), itmp2);
                v_inm2+=2;
                v_inp2+=2;
            }

            while(h < nn - 1) {
                out[h] = in[i - 1] - ((wav_t)(in[i - 2] + in[i] + 1) >> 1);
                out[l] = in[i - 2] + ((wav_t)(out[h - 1] + out[h] + 2) >> 2);
                h++;
                l++;
                i+=2;
            }

            /* Final prediction */
            out[h] = in[i - 1];
            /* Final update */
            out[l] = in[i - 2] + ((wav_t)(out[h - 1] + out[h] + 2) >> 2);

            memcpy(in, out, nn*sizeof(wav_t));
        }

        /********************************* LINES ****************************/
        for(x = 0; x < width; x+=WB_PER_128b) {
            /* Compute new length / 2 */
            ns2 = nn / 2;

            v_outl = (__m128i*)&mat_out[x];
            v_outh = (__m128i*)&mat_out[x + ns2*width];
            v_outhm1 = (__m128i*)&mat_out[x + (ns2 - 1)*width];

            v_in = (__m128i*)&mat_in[x + 2*width];
            v_inm1 = (__m128i*)&mat_in[x + width];
            v_inm2 = (__m128i*)&mat_in[x];

            /* Note: splitting is done while predicting & updating */
            i = 2;
            /* High & low frequencies offsets */
            l = 0;
            in2 = _mm_load_si128(v_inm2);
            /* Predictions & updates */
            h = ns2;
            while(h < nn - 1) {
                /* Predictions & updates */
                in0 = _mm_load_si128(v_in);
                in1 = _mm_load_si128(v_inm1);
                itmp3 = _mm_load_si128(v_outhm1);
#ifdef WAVT_16
                itmp1 = _mm_add_epi16(in2,  in0);
                itmp1 = _mm_add_epi16(itmp1, ones);
                itmp1 = _mm_srai_epi16(itmp1, 1);
                itmp1 = _mm_sub_epi16(in1, itmp1);
                _mm_store_si128(v_outh, itmp1);

                itmp2 = _mm_add_epi16(itmp3, itmp1);
                itmp2 = _mm_add_epi16(itmp2, twos);
                itmp2 = _mm_srai_epi16(itmp2, 2);
                itmp1 = _mm_add_epi16(in2, itmp2);
                _mm_store_si128(v_outl, itmp1);
#else
                itmp1 = _mm_add_epi32(in2,  in0);
                itmp1 = _mm_add_epi32(itmp1, ones);
                itmp1 = _mm_srai_epi32(itmp1, 1);
                itmp1 = _mm_sub_epi32(in1, itmp1);
                _mm_store_si128(v_outh, itmp1);

                itmp2 = _mm_add_epi32(itmp3, itmp1);
                itmp2 = _mm_add_epi32(itmp2, twos);
                itmp2 = _mm_srai_epi32(itmp2, 2);
                itmp1 = _mm_add_epi32(in2, itmp2);
                _mm_store_si128(v_outl, itmp1);
#endif
                i+=2;
                l++;
                h++;
                v_in += width*2/WB_PER_128b;
                v_inm1 += width*2/WB_PER_128b;
                v_outl += width/WB_PER_128b;
                v_outh += width/WB_PER_128b;
                v_outhm1 += width/WB_PER_128b;
                in2 = in0;
            }
        }
        for(x = 0; x < width; x++) {
            /* Initial prediction & update */
            mat_out[x + ns2*width] = mat_in[x + width] - 
                                   ((wav_t)(mat_in[x] + 
                                            mat_in[x + 2*width] + 1) >> 1);
            mat_out[x] = mat_in[x];

            /* Final prediction & update */
            mat_out[x+(nn - 1)*width] = mat_in[x+(nn - 1)*width];
            mat_out[x+(ns2 - 1)*width] = 
                mat_in[x+(nn - 2)*width] + 
                ((wav_t)(mat_out[x+(nn - 2)*width] + 
                         mat_out[x+(nn - 1)*width] + 2) >> 2);
        }
        if (k != 0)
            memcpy(mat_in, mat_out, width*nn*sizeof(wav_t));
        nn /= 2;
    }
}

/**
 * \brief Inverse IWT (5,3) in 2-Dimensions
 * \param out output
 * \param in input
 * \param n input length
 * \param p Codec parameters
 */
void
iiwt2D_5_3(wav_t *mat_out, wav_t *mat_in, wavparams_t * p) {
    ssize_t i, x, y, k, h, l;
    ssize_t ns2, nn;
    size_t width = p->w;
    size_t height = p->h;
    wav_t *in;
    wav_t *out;
    wav_t tmp;

    __m128i inh, inl, outim1, outi, itmp1, itmp2, itmp3, last_op1;
#ifdef WAVT_16
    __m128i ones = _mm_set1_epi16(1);
    __m128i twos = _mm_set1_epi16(2);
#else
    __m128i ones = _mm_set1_epi32(1);
    __m128i twos = _mm_set1_epi32(2);
#endif /* WAVT_16 */
    __m128i *v_inhm1, *v_inh, *v_inl;
    __m128i *v_outi, *v_outim1, *v_outip1;

    nn  = width / pow(2, p->nb_levels - 1);
    ns2 = nn / 2;
    for (k = p->nb_levels - 1; k >= 0; k--) {
        /********************************* LINES ****************************/
        for(x = 0; x < width; x++) {
            out = &mat_out[x];
            in  = &mat_in[x];

            /* Initial update */
            out[(nn - 2)*width] = in[(ns2 - 1)*width] - 
                              ((wav_t)(in[(nn - 2)*width] + 
                                       in[(nn - 1)*width] + 2) >> 2);
            out[(nn - 1)*width] = in[(nn - 1)*width];
        }
        for(x = 0; x < width; x+=WB_PER_128b) {
            out = &mat_out[x];
            in  = &mat_in[x];

            /* Note: merging is done when reversing primary & dual lifting */
            /* High & low frequencies offsets */
            h = nn - 2;
            l = ns2 - 2;
            /* output offsets */
            i = nn - 3;
            v_inl = (__m128i*)&mat_in[x + l*width];
            v_inh = (__m128i*)&mat_in[x + h*width];
            v_inhm1 = (__m128i*)&mat_in[x + (h - 1)*width];
            v_outi = (__m128i*)&mat_out[x + i*width];
            v_outim1 = (__m128i*)&mat_out[x + (i - 1)*width];
            v_outip1 = (__m128i*)&mat_out[x + (i + 1)*width];
            while(l > 0) {
                /* Inverse primary lifting */
                inl = _mm_load_si128(v_inl);
                inh = _mm_load_si128(v_inh);
                itmp3 = _mm_load_si128(v_inhm1);
                itmp2 = _mm_load_si128(v_outip1);

#ifdef WAVT_16
                itmp1 = _mm_add_epi16(itmp3,  inh);
                itmp1 = _mm_add_epi16(itmp1, twos);
                itmp1 = _mm_srai_epi16(itmp1, 2);
                itmp1 = _mm_sub_epi16(inl, itmp1);
                _mm_store_si128(v_outim1, itmp1);

                itmp1 = _mm_add_epi16(itmp2, itmp1);
                itmp1 = _mm_add_epi16(itmp1, ones);
                itmp1 = _mm_srai_epi16(itmp1, 1);
                itmp1 = _mm_add_epi16(inh, itmp1);
                _mm_store_si128(v_outi, itmp1);
#else
                itmp1 = _mm_add_epi32(itmp3,  inh);
                itmp1 = _mm_add_epi32(itmp1, twos);
                itmp1 = _mm_srai_epi32(itmp1, 2);
                itmp1 = _mm_sub_epi32(inl, itmp1);
                _mm_store_si128(v_outim1, itmp1);

                itmp1 = _mm_add_epi32(itmp2, itmp1);
                itmp1 = _mm_add_epi32(itmp1, ones);
                itmp1 = _mm_srai_epi32(itmp1, 1);
                itmp1 = _mm_add_epi32(inh, itmp1);
                _mm_store_si128(v_outi, itmp1);
#endif
                l--;
                v_inl -= width/WB_PER_128b;
                v_inh -= width/WB_PER_128b;
                v_inhm1 -= width/WB_PER_128b;
                v_outi -= 2*width/WB_PER_128b;
                v_outim1 -= 2*width/WB_PER_128b;
                v_outip1 -= 2*width/WB_PER_128b;
            }
        }
        for(x = 0; x < width; x++) {
            out = &mat_out[x];
            in  = &mat_in[x];

            /* Final update */
            out[0] = in[0];
            out[width] = in[ns2*width] + 
                         ((wav_t)(out[0] + out[2*width] + 1) >> 1);
        }
        memcpy(mat_in, mat_out, nn*height*sizeof(wav_t));

      /******************************** COLUMNS *****************************/
      for(y = 0; y < height; y++) {
        out = &mat_out[y*width];
        in  = &mat_in[y*width];

        /* Note: merging is done when reversing primary & dual lifting */
        /* High & low frequencies offsets */
        h = nn - 1;
        l = ns2 - 1;
        /* output offsets */
        i = nn - 1;
        out[i - 1] = in[l] - ((wav_t)(in[h - 1] + in[h] + 2) >> 2);
        out[i] = in[h];
        i -= 2;
        h--;
        l--;
        /* 16-bytes align output buffer */
        while( l > ns2 - 5) {
            /* Inverse primary lifting */
            out[i - 1] = in[l] - ((wav_t)(in[h - 1] + in[h] + 2) >> 2);
            /* Inverse dual lifting */
            out[i] = in[h] + ((wav_t)(out[i - 1] + out[i + 1] + 1) >> 1);
            i -= 2;
            h--;
            l--;
        }
        /* Ouput buffer now 16 bytes aligned (speed optimisation) */
        v_inhm1 = (__m128i*)(in + h - WB_PER_128b);
        v_inh = (__m128i*)(in + h - WB_PER_128b + 1);
        v_inl = (__m128i*)(in + l - WB_PER_128b + 1);
        while(l > WB_PER_128b) {
            itmp1 = _mm_loadu_si128(v_inhm1);
            inh = _mm_loadu_si128(v_inh);
            inl = _mm_loadu_si128(v_inl);
#ifdef WAVT_16
            /* Inverse primary lifting */
            itmp1 = _mm_add_epi16(itmp1,  inh);
            itmp1 = _mm_add_epi16(itmp1,  twos);
            itmp1 = _mm_srai_epi16(itmp1, 2);
            outim1 = _mm_sub_epi16(inl, itmp1);

            tmp = in[l + 1] - ((wav_t)(in[h] + in[h + 1] + 2) >> 2);
            last_op1 = _mm_srli_si128(outim1, WAVT_BYTE_SIZE);
            last_op1 = _mm_insert_epi16 (last_op1, tmp, 7);

            /* Inverse dual lifting */
            itmp1 = _mm_add_epi16(outim1, last_op1);
            itmp1 = _mm_add_epi16(itmp1, ones);
            itmp1 = _mm_srai_epi16(itmp1, 1);
            outi = _mm_add_epi16(inh, itmp1);

            itmp1 = _mm_unpacklo_epi16(outim1, outi);
            _mm_store_si128((__m128i*)(out +  i - 2*WB_PER_128b + 1), itmp1);
            itmp1 = _mm_unpackhi_epi16(outim1, outi);
            _mm_store_si128((__m128i*)(out +  i - WB_PER_128b + 1), itmp1);
#else
            /* Inverse primary lifting */
            itmp1 = _mm_add_epi32(itmp1,  inh);
            itmp1 = _mm_add_epi32(itmp1,  twos);
            itmp1 = _mm_srai_epi32(itmp1, 2);
            outim1 = _mm_sub_epi32(inl, itmp1);

            tmp = in[l + 1] - ((wav_t)(in[h] + in[h + 1] + 2) >> 2);
            last_op1 = _mm_srli_si128(outim1, WAVT_BYTE_SIZE);
            last_op1 = _mm_insert_epi16 (last_op1, tmp, 6);
            last_op1 = _mm_insert_epi16 (last_op1, tmp>>16, 7);

            /* Inverse dual lifting */
            itmp1 = _mm_add_epi32(outim1, last_op1);
            itmp1 = _mm_add_epi32(itmp1, ones);
            itmp1 = _mm_srai_epi32(itmp1, 1);
            outi = _mm_add_epi32(inh, itmp1);

            itmp1 = _mm_unpacklo_epi32(outim1, outi);
            _mm_store_si128((__m128i*)(out +  i - 2*WB_PER_128b + 1), itmp1);
            itmp1 = _mm_unpackhi_epi32(outim1, outi);
            _mm_store_si128((__m128i*)(out +  i - WB_PER_128b + 1), itmp1);
#endif
            v_inhm1--;
            v_inh--;
            v_inl--;
            l-=WB_PER_128b;
            h-=WB_PER_128b;
            i-=2*WB_PER_128b;
        }
        while(l > 0) {
            /* Inverse primary lifting */
            out[i - 1] = in[l] - ((wav_t)(in[h - 1] + in[h] + 2) >> 2);
            /* Inverse dual lifting */
            out[i] = in[h] + ((wav_t)(out[i - 1] + out[i + 1] + 1) >> 1);
            i -= 2;
            h--;
            l--;
        }
        i = 1;
        l = 0;
        h = ns2;
        /* Finalize update */
        out[i - 1] = in[l];
        out[i] = in[h] + ((wav_t)(out[i - 1] + out[i + 1] + 1) >> 1);

        /* Can't find a better optimization... */
        if (k != 0)
         memcpy(in, out, nn*sizeof(wav_t));
      }

        /* Compute new length * 2 */
        ns2 *= 2;
        nn *= 2;
    }
}

/**
 * \brief Forward IWT (5,3) in 1-Dimension
 * \param out output
 * \param in input   (will be modified ! Out: Contains the step before last 
 *                    level decomposition)
 * \param n input length (must be a [power of 2] > 4)
 * \param p Codec parameters
 */
void
fiwt1D_5_3(wav_t *out, wav_t *in, int n, wavparams_t * p) {
   ssize_t i, k, h, l = 0;
   ssize_t nn = n, ns2 = n;
   __m128i in0, in1, in2, itmp1, itmp2, itmp3, itmp4;
#ifdef WAVT_16
   __m128i ones = _mm_set1_epi16(1);
   __m128i twos = _mm_set1_epi16(2);
#else
   __m128i ones = _mm_set1_epi32(1);
   __m128i twos = _mm_set1_epi32(2);
#endif /* WAVT_16 */
   __m128i *v_inm2, *v_inp2;

   /* Do a n_level cut iteration */
   for (k = p->nb_levels - 1; k >= 0; k--) {
      /* Compute new length / 2 */
      ns2 /= 2;
      /* Note: splitting is done while predicting & updating */
      i = 2;
      /* High & low frequencies offsets */
      l = 0;
      h = ns2;
      /* Primary prediction */
      out[h] = in[i - 1] - ((wav_t)(in[i - 2] + in[i] + 1) >> 1);
      /* Primary update */
      out[l] = in[i - 2];
      i+=2;
      l++;
      for(h = ns2 + 1; h < ns2 + 8; h++,l++,i+=2) {
         /* Predictions */
         out[h] = in[i - 1] - ((wav_t)(in[i - 2] + in[i] + 1) >> 1);
         /* Updates */
         out[l] = in[i - 2] + ((wav_t)(out[h - 1] + out[h] + 2) >> 2);
      }
      v_inm2 = (__m128i*)(in + 2*8);
      v_inp2 = (__m128i*)(in + WB_PER_128b + 2*8);
      /* Predictions & updates */
      for (h = ns2 + 8; h < nn - WB_PER_128b - 1; h+=WB_PER_128b, 
                                                  l+=WB_PER_128b, 
                                                  i+=2*WB_PER_128b) {
         itmp1 = _mm_load_si128(v_inm2);
         itmp2 = _mm_load_si128(v_inp2);
         /* Predictions with SIMD power :) */
/*         out[h] = in[i - 1] - ((in[i - 2] + in[i] + 1) / 2);*/
#ifdef WAVT_16
         /* 1 2 3 4 5 6 7 8 */
         itmp3 = _mm_shufflelo_epi16(itmp1, _MM_SHUFFLE(3, 1, 2, 0));
         /* 1 3 2 4 5 7 6 8 */
         itmp3 = _mm_shufflehi_epi16(itmp3, _MM_SHUFFLE(3, 1, 2, 0));
         /* 1 3 5 7 2 4 6 8 */
         itmp3 = _mm_shuffle_epi32(itmp3, _MM_SHUFFLE(3, 1, 2, 0));
         /* Idem */
         itmp4 = _mm_shufflelo_epi16(itmp2, _MM_SHUFFLE(3, 1, 2, 0));
         itmp4 = _mm_shufflehi_epi16(itmp4, _MM_SHUFFLE(3, 1, 2, 0));
         itmp4 = _mm_shuffle_epi32(itmp4, _MM_SHUFFLE(3, 1, 2, 0));
         in2 = _mm_unpacklo_epi64(itmp3, itmp4);
         in1 = _mm_unpackhi_epi64(itmp3, itmp4);

         itmp1 = _mm_srli_si128 (in2, WAVT_BYTE_SIZE);
         in0 = _mm_insert_epi16 (itmp1, in[i + 2*WB_PER_128b - 2], 7);
         itmp1 = _mm_add_epi16(in2,  in0);
         itmp1 = _mm_add_epi16(itmp1, ones);
         itmp1 = _mm_srai_epi16(itmp1, 1);
         itmp1 = _mm_sub_epi16(in1, itmp1);
         _mm_store_si128((__m128i*)(out + h), itmp1);

         /* Updates */
/*         out[l] = in[i - 2] + ((out[h - 1] + out[h] + 2) / 4);*/
         itmp3 = _mm_loadu_si128((__m128i*)(out + h - 1));
         itmp2 = _mm_add_epi16(itmp3, itmp1);
         itmp2 = _mm_add_epi16(itmp2, twos);
         itmp2 = _mm_srai_epi16(itmp2, 2);
         itmp2 = _mm_add_epi16(in2, itmp2);
#else
         itmp3 = _mm_shuffle_epi32(itmp1, _MM_SHUFFLE(3, 1, 2, 0));
         itmp4 = _mm_shuffle_epi32(itmp2, _MM_SHUFFLE(3, 1, 2, 0));
         in2 = _mm_unpacklo_epi64(itmp3, itmp4);
         in1 = _mm_unpackhi_epi64(itmp3, itmp4);

         itmp1 = _mm_srli_si128 (in2, WAVT_BYTE_SIZE);
         itmp1 = _mm_insert_epi16 (itmp1, in[i + 2*WB_PER_128b - 2], 6);
         in0 = _mm_insert_epi16 (itmp1, in[i + 2*WB_PER_128b - 2]>>16, 7);

         itmp1 = _mm_add_epi32(in2,  in0);
         itmp1 = _mm_add_epi32(itmp1, ones);
         itmp1 = _mm_srai_epi32(itmp1, 1);
         itmp1 = _mm_sub_epi32(in1, itmp1);
         _mm_store_si128((__m128i*)(out + h), itmp1);

         /* Updates */
/*         out[l] = in[i - 2] + ((out[h - 1] + out[h] + 2) / 4);*/
         itmp3 = _mm_loadu_si128((__m128i*)(out + h - 1));
         itmp2 = _mm_add_epi32(itmp3, 
                               itmp1);
         itmp2 = _mm_add_epi32(itmp2, twos);
         itmp2 = _mm_srai_epi32(itmp2, 2);
         itmp2 = _mm_add_epi32(in2, itmp2);
#endif   /* WAVT_16 */
         _mm_store_si128((__m128i*)(out +  l), itmp2);
         v_inm2+=2;
         v_inp2+=2;
      }

      while(h < nn - 1) {
         out[h] = in[i - 1] - ((wav_t)(in[i - 2] + in[i] + 1) >> 1);
         out[l] = in[i - 2] + ((wav_t)(out[h - 1] + out[h] + 2) >> 2);
         h++;
         l++;
         i+=2;
      }

      /* Final prediction */
      out[h] = in[i - 1];
      /* Final update */
      out[l] = in[i - 2] + ((wav_t)(out[h - 1] + out[h] + 2) >> 2);

      nn /= 2;
      /* Can't find a better optimization... */
      if (k != 0) {
         memcpy(in, out, nn*sizeof(wav_t));
      }
   }
}

/**
 * \brief Inverse IWT (5,3)
 * \param out output
 * \param in input
 * \param n input length
 * \param p Codec parameters
 */
void
iiwt1D_5_3(wav_t *out, wav_t *in, int n, wavparams_t * p) {
    ssize_t i, k, h, l;
    ssize_t nn = n / pow(2, p->nb_levels - 1);
    ssize_t ns2 = nn / 2;
    wav_t tmp;

    __m128i inh, inl, outim1, outi, itmp1, last_op1;
#ifdef WAVT_16
    __m128i ones = _mm_set1_epi16(1);
    __m128i twos = _mm_set1_epi16(2);
#else
    __m128i ones = _mm_set1_epi32(1);
    __m128i twos = _mm_set1_epi32(2);
#endif /* WAVT_16 */
    __m128i *v_inhm1, *v_inh, *v_inl;
    for (k = p->nb_levels - 1; k >= 0; k--) {
        /* Note: merging is done when reversing primary & dual lifting */
        /* High & low frequencies offsets */
        h = nn - 1;
        l = ns2 - 1;
        /* output offsets */
        i = nn - 1;
        out[i - 1] = in[l] - ((wav_t)(in[h - 1] + in[h] + 2) >> 2);
        out[i] = in[h];
        i -= 2;
        h--;
        l--;
        /* 16-bytes align output buffer */
        while( l > ns2 - 5) {
            /* Inverse primary lifting */
            out[i - 1] = in[l] - ((wav_t)(in[h - 1] + in[h] + 2) >> 2);
            /* Inverse dual lifting */
            out[i] = in[h] + ((wav_t)(out[i - 1] + out[i + 1] + 1) >> 1);
            i -= 2;
            h--;
            l--;
        }
        /* Ouput buffer now 16 bytes aligned (speed optimisation) */
        v_inhm1 = (__m128i*)(in + h - WB_PER_128b);
        v_inh = (__m128i*)(in + h - WB_PER_128b + 1);
        v_inl = (__m128i*)(in + l - WB_PER_128b + 1);
        while(l > WB_PER_128b) {
            itmp1 = _mm_loadu_si128(v_inhm1);
            inh = _mm_loadu_si128(v_inh);
            inl = _mm_loadu_si128(v_inl);
#ifdef WAVT_16
            /* Inverse primary lifting */
            itmp1 = _mm_add_epi16(itmp1,  inh);
            itmp1 = _mm_add_epi16(itmp1,  twos);
            itmp1 = _mm_srai_epi16(itmp1, 2);
            outim1 = _mm_sub_epi16(inl, itmp1);

            tmp = in[l + 1] - ((wav_t)(in[h] + in[h + 1] + 2) >> 2);
            last_op1 = _mm_srli_si128(outim1, WAVT_BYTE_SIZE);
            last_op1 = _mm_insert_epi16 (last_op1, tmp, 7);

            /* Inverse dual lifting */
            itmp1 = _mm_add_epi16(outim1, last_op1);
            itmp1 = _mm_add_epi16(itmp1, ones);
            itmp1 = _mm_srai_epi16(itmp1, 1);
            outi = _mm_add_epi16(inh, itmp1);

            itmp1 = _mm_unpacklo_epi16(outim1, outi);
            _mm_store_si128((__m128i*)(out +  i - 2*WB_PER_128b + 1), itmp1);
            itmp1 = _mm_unpackhi_epi16(outim1, outi);
            _mm_store_si128((__m128i*)(out +  i - WB_PER_128b + 1), itmp1);
#else
            /* Inverse primary lifting */
            itmp1 = _mm_add_epi32(itmp1,  inh);
            itmp1 = _mm_add_epi32(itmp1,  twos);
            itmp1 = _mm_srai_epi32(itmp1, 2);
            outim1 = _mm_sub_epi32(inl, itmp1);

            tmp = in[l + 1] - ((wav_t)(in[h] + in[h + 1] + 2) >> 2);
            last_op1 = _mm_srli_si128(outim1, WAVT_BYTE_SIZE);
            last_op1 = _mm_insert_epi16 (last_op1, tmp, 6);
            last_op1 = _mm_insert_epi16 (last_op1, tmp>>16, 7);

            /* Inverse dual lifting */
            itmp1 = _mm_add_epi32(outim1, last_op1);
            itmp1 = _mm_add_epi32(itmp1, ones);
            itmp1 = _mm_srai_epi32(itmp1, 1);
            outi = _mm_add_epi32(inh, itmp1);

            itmp1 = _mm_unpacklo_epi32(outim1, outi);
            _mm_store_si128((__m128i*)(out +  i - 2*WB_PER_128b + 1), itmp1);
            itmp1 = _mm_unpackhi_epi32(outim1, outi);
            _mm_store_si128((__m128i*)(out +  i - WB_PER_128b + 1), itmp1);
#endif
            v_inhm1--;
            v_inh--;
            v_inl--;
            l-=WB_PER_128b;
            h-=WB_PER_128b;
            i-=2*WB_PER_128b;
        }
        while(l > 0) {
            /* Inverse primary lifting */
            out[i - 1] = in[l] - ((wav_t)(in[h - 1] + in[h] + 2) >> 2);
            /* Inverse dual lifting */
            out[i] = in[h] + ((wav_t)(out[i - 1] + out[i + 1] + 1) >> 1);
            i -= 2;
            h--;
            l--;
        }
        i = 1;
        l = 0;
        h = ns2;
        /* Finalize update */
        out[i - 1] = in[l];
        out[i] = in[h] + ((wav_t)(out[i - 1] + out[i + 1] + 1) >> 1);

        /* Can't find a better optimization... */
        if (k != 0)
         memcpy(in, out, nn*sizeof(wav_t));

        /* Compute new length * 2 */
        ns2 *= 2;
        nn *= 2;
   }
}


/**
 * \brief Forward IWT (5,3) in 2-Dimensions
 * \param out output
 * \param in input   (will be modified ! Out: Contains the step before last 
 *                    level decomposition)
 * \param n input length
 * \param p Codec parameters
 */
void
fiwt2D_5_3_slow(wav_t *mat_out, wav_t *mat_in, wavparams_t * p) {
    ssize_t i, x, y, k, h, l = 0;
    ssize_t nn, ns2, nnm1;
    size_t width = p->w;
    size_t height = p->h;
    wav_t *in = NULL;
    wav_t *in_i;
    wav_t *in_im1;
    wav_t *in_im2;
    wav_t *out = NULL;
    wav_t *out_l;
    wav_t *out_h;

    /* Do a n_level cut iteration */

    nn = width;
    ns2 = width;
    for (k = p->nb_levels - 1; k >= 0; k--) {
        /******************************** COLUMNS ***************************/
        for(y = 0; y < height; y++) {
            /* Can't find a better optimization... */
            out = &mat_out[y*width];
            in  = &mat_in[y*width];
            /* Compute new length / 2 */
            ns2 = nn / 2;
            nnm1 = nn - 1;

            /* Note: splitting is done while predicting & updating */
            i = 2;
            /* High & low frequencies offsets */
            l = 0;
            h = ns2;
            /* Using rolled image for first and last index 
              (see Le Gall algorithm) */
            /* Final prediction */
            out[nn - 1] = in[nn - 1] - in[nn - 2];
            /* Primary prediction */
            out[ns2] = in[1] - ((in[0] + in[2] + 1) >> 1);
            i+=2;
            l++;
            /* Predictions & updates */
            for (h = ns2+1; h < nnm1; h++) {
                /* Predictions */
                out[h] = in[i - 1] - ((in[i - 2] + in[i] + 1) / 2);
                /* Updates */
                out[l] = in[i - 2] + ((out[h - 1] + out[h] + 2) / 4);
                i+=2;
                l++;
            }
            /* Final update */
            out[ns2 - 1] = in[nn - 2] + 
                        ((out[nn - 2] + out[nn - 1] + 2) >> 2);
            /* Primary update */
            out[0] = in[0] + out[ns2];

            memcpy(in, out, nn*sizeof(wav_t));
        }
        /********************************* LINES ****************************/
        for(x = 0; x < width; x++) {
            /* Compute new length / 2 */
            ns2 = nn / 2;
            nnm1 = nn - 1;

            out = &mat_out[x];
            out_l = &mat_out[x];
            out_h = &mat_out[x+ns2*width];
            in  = &mat_in[x];
            in_i = &mat_in[x+2*width];
            in_im1 = &mat_in[x+width];
            in_im2 = &mat_in[x];

            /* Note: splitting is done while predicting & updating */
            i = 2;
            /* High & low frequencies offsets */
            l = 0;
            h = ns2;

            /* Using rolled image for first and last index 
              (see Le Gall algorithm) */
            /* Final prediction */
            out[(nn - 1)*width] = in[(nn - 1)*width] - in[(nn - 2)*width];
            /* Primary prediction */
            out[ns2*width] = in[width] - ((in[0] + in[2*width] + 1) >> 1);

            i+=2;
            in_i += width*2;
            in_im1 += width*2;
            in_im2 += width*2;
            out_l+=width;
            out_h+=width;
            l++;
            /* Predictions & updates */
            for (h = ns2+1; h < nnm1; h++) {
                /* Predictions */
                *out_h = *in_im1 - ((*in_im2 + *in_i + 1) / 2);
                /* Updates */
                *out_l = *in_im2 + ((out[(h - 1)*width] + *out_h + 2) / 4);
                i+=2;
                l++;
                in_i += width*2;
                in_im1 += width*2;
                in_im2 += width*2;
                out_l += width;
                out_h += width;
            }
            out = &mat_out[x];
            in  = &mat_in[x];
            /* Final update */
            out[(ns2 - 1)*width] = in[(nn - 2)*width] + 
                            ((out[(nn - 2)*width] + 
                            out[(nn - 1)*width] + 2) >> 2);
            /* Primary update */
            out[0] = in[0] + out[ns2*width];
        }
        if (k != 0)
            memcpy(mat_in, mat_out, nn*width*sizeof(wav_t));

        nn /= 2;
    }
}

/**
 * \brief Inverse IWT (5,3) in 2-Dimensions
 * \param out output
 * \param in input
 * \param n input length
 * \param p Codec parameters
 */
void
iiwt2D_5_3_slow(wav_t *mat_out, wav_t *mat_in, wavparams_t * p) {
    ssize_t i, x, y, k, h, l;
    ssize_t nn;
    ssize_t ns2, ns2m1;
    size_t width = p->w;
    size_t height = p->h;
    wav_t *in;
    wav_t *out;

    nn  = width / pow(2, p->nb_levels - 1);
    ns2 = nn / 2;
    for (k = p->nb_levels - 1; k >= 0; k--) {
        /********************************* LINES ******************************/
        for(x = 0; x < width; x++) {
            out = &mat_out[x];
            in  = &mat_in[x];
            ns2m1 = ns2 - 1;
            /* Note: merging is done when reversing primary & dual lifting */
            /* High & low frequencies offsets */
            h = nn - 1;
            l = ns2m1;
            /* output offsets */
            i = nn - 1;
            /* Using rolled image for first and last index 
              (see Le Gall algorithm) */
            /* Inverse the first primary lifting */
            out[0] = in[0] - in[ns2*width];
            /* Inverse last primary lifting */
            out[(nn - 2)*width] = in[(ns2 - 1)*width] - 
                        ((in[(nn - 2)*width] + in[(nn - 1)*width] + 2) >> 2);
            /* Inverse last dual lifting */
            out[(nn - 1)*width] = in[(nn - 1)*width] + out[(nn - 2)*width];
            i -= 2;
            h--;
            for (l = ns2m1 - 1; l > 0; l--) {

                /* Inverse primary lifting */
                out[(i - 1)*width] = in[l*width] - 
                                     ((in[(h - 1)*width] + in[h*width] + 2) / 4);
                /* Inverse dual lifting */
                out[i*width] = in[h*width] + 
                               ((out[(i - 1)*width] + out[(i + 1)*width] + 1)/2);

                i -= 2;
                h--;
            }
            /* Inverse the first dual lifting */
            out[width] = in[ns2*width] + ((out[0] + out[2*width] + 1) >> 1);
        }
        memcpy(mat_in, mat_out, nn*height*sizeof(wav_t));
        /******************************** COLUMNS *****************************/
        for(y = 0; y < height; y++) {
            out = &mat_out[y*width];
            in  = &mat_in[y*width];
            ns2m1 = ns2 - 1;
            /* Note: merging is done when reversing primary & dual lifting */
            /* High & low frequencies offsets */
            h = nn - 1;
            l = ns2m1;
            /* output offsets */
            i = nn - 1;
            /* Using rolled image for first and last index 
              (see Le Gall algorithm) */
            /* Inverse the first primary lifting */
            out[0] = in[0] - in[ns2];
            /* Inverse last primary lifting */
            out[nn - 2] = in[ns2 - 1] - ((in[nn - 2] + in[nn - 1] + 2) >> 2);
            /* Inverse last dual lifting */
            out[nn - 1] = in[nn - 1] + out[nn - 2];
            i -= 2;
            h--;
            for (l = ns2m1 - 1; l > 0; l--) {
                /* Inverse primary lifting */
                out[i - 1] = in[l] - ((in[h - 1] + in[h] + 2) / 4);
                /* Inverse dual lifting */
                out[i] = in[h] + ((out[i - 1] + out[i + 1] + 1) / 2);
                i -= 2;
                h--;
            }
            /* Inverse the first dual lifting */
            out[1] = in[ns2] + ((out[0] + out[2] + 1) >> 1);

            /* Can't find a better optimization... */
            if (k != 0)
                memcpy(in, out, nn*sizeof(wav_t));
        }


            /* Compute new length * 2 */
            ns2 *= 2;
            nn *= 2;
    }
}

/**
 * \brief Forward IWT (5,3) in 1-Dimension
 * \param out output
 * \param in input   (will be modified ! Out: Contains the step before last 
 *                    level decomposition)
 * \param n input length (must be a [power of 2] > 4)
 * \param p Codec parameters
 */
void
fiwt1D_5_3_slow(wav_t *out, wav_t *in, int n, wavparams_t * p) {
    ssize_t i, k, h, l = 0;
    ssize_t nn = n, ns2 = n, nnm1;

    /* Do a n_level cut iteration */
    for (k = p->nb_levels - 1; k >= 0; k--) {
        /* Compute new length / 2 */
        ns2 /= 2;
        nnm1 = nn - 1;
        /* Note: splitting is done while predicting & updating */
        i = 2;
        /* High & low frequencies offsets */
        l = 0;
        h = ns2;
        /* Using rolled image for first and last index 
          (see Le Gall algorithm) */
        /* Final prediction */
        out[nn - 1] = in[nn - 1] - in[nn - 2];
        /* Primary prediction */
        out[ns2] = in[1] - ((wav_t)(in[0] + in[2] + 1) >> 1);
        /* Primary update */
        out[0] = in[0] + out[ns2];
        i+=2;
        l++;
        /* Predictions & updates */
        for (h = ns2+1; h < nnm1; h++) {
            /* Predictions */
            out[h] = in[i - 1] - ((wav_t)(in[i - 2] + in[i] + 1) >> 1);
            /* Updates */
            out[l] = in[i - 2] + ((wav_t)(out[h - 1] + out[h] + 2) >> 2);
            i+=2;
            l++;
        }
        /* Final update */
        out[ns2 - 1] = in[nn - 2] + ((wav_t)(out[nn - 2] + 
                                             out[nn - 1] + 2) >> 2);

        nn /= 2;
        /* Can't find a better optimization... */
        if (k != 0)
            memcpy(in, out, nn*sizeof(wav_t));
    }
}


/**
 * \brief Inverse IWT (5,3)
 * \param out output
 * \param in input
 * \param n input length
 * \param p Codec parameters
 */
void
iiwt1D_5_3_slow(wav_t *out, wav_t *in, int n, wavparams_t * p) {
    int i, k, h, l;
    int nn = n / pow(2, p->nb_levels - 1);
    int ns2 = nn / 2, ns2m1;

    for (k = p->nb_levels - 1; k >= 0; k--) {
        /* Note: merging is done when reversing primary & dual lifting */

        ns2m1 = ns2 - 1;

        /* High & low frequencies offsets */
        h = nn - 1;
        l = ns2m1;
        /* output offsets */
        i = nn - 1;
        /* Using rolled image for first and last index 
          (see Le Gall algorithm) */
        /* Inverse the first primary lifting */
        out[0] = in[0] - in[ns2];
        /* Inverse last primary lifting */
        out[nn - 2] = in[ns2 - 1] - ((wav_t)(in[nn - 2] + in[nn - 1] + 2) >> 2);
        /* Inverse last dual lifting */
        out[nn - 1] = in[nn - 1] + out[nn - 2];
        i -= 2;
        h--;
        for (l = ns2m1 - 1; l > 0; l--) {
            /* Inverse primary lifting */
            out[i - 1] = in[l] - ((wav_t)(in[h - 1] + in[h] + 2) >> 2);
            /* Inverse dual lifting */
            out[i] = in[h] + ((wav_t)(out[i - 1] + out[i + 1] + 1) >> 1);
            i -= 2;
            h--;
        }
        /* Inverse the first dual lifting */
        out[1] = in[ns2] + ((wav_t)(out[0] + out[2] + 1) >> 1);

        /* Can't find a better optimization... */
        if (k != 0)
            memcpy(in, out, nn*sizeof(wav_t));

        /* Compute new length * 2 */
        ns2 *= 2;
        nn *= 2;
    }
}
