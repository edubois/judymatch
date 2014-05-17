#include <unistd.h>
#include "vfwl_globals.h"

#ifndef VFWL_IMAGE_H
#define VFWL_IMAGE_H
#ifdef WAVT_16
    typedef short pix_t;
#else
    typedef int pix_t;
#endif


typedef enum color_space {
    IT_Unknown, IT_Mono, IT_CIEY, 
    IT_CIEL, IT_RGB, IT_CIEXYZ, 
    IT_CIELab, IT_YUV, IT_YUVr
} color_space;

typedef struct image_t {
    color_space col_space;
    size_t w, h;
    int bpp;
    pix_t *channels;
    int nb_channels;
} image_t;

pix_t to_pixel(double value);
void vfwl_init_image(image_t *out);
void vfwl_alloc_image(image_t *out, size_t w, size_t h, int nb_channels);
void vfwl_import_pnm(image_t *out, char *filename);
void vfwl_export_pnm(char *filename, image_t *in, int raw);
void vfwl_convert_RGB_to_YCbCr(image_t *in);
void vfwl_convert_YCbCr_to_RGB(image_t *in);
void vfwl_free_image(image_t *in);
#endif  /* VFWL_IMAGE_H */
