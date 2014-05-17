#include <emmintrin.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "vfwl_image.h"

/** ================================ PRIVATE ============================= **/
static unsigned char* 
get_active_char(unsigned char *c, unsigned char *buffer) {
    buffer++;
    while(*buffer == '#') {
        do
            buffer++;
        while(*buffer != '\n');
        buffer++;
    }
    *c = *buffer;
    return buffer;
}

/*  Skip Leading whitespace and read an ASCII Number from a PNM File */
static unsigned char* 
get_active_number(unsigned int *val, unsigned char *buffer) {
    unsigned int value = 0;
    unsigned char c;
    do
        buffer = get_active_char(&c, buffer);
    while ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'));
    while (1) {
        if ((c >= '0') && (c <= '9')) {
            value = 10 * value + (c - '0');
            buffer = get_active_char(&c, buffer);
            continue;
        }
        if ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'))
            break;
        fprintf(stderr, "%s: Not a number\n", __FUNCTION__);
    }
    *val = value;
    return buffer;
}

pix_t 
to_pixel(double value) {
    if (value > 255.0)
        value = 255.0;
    else if (value < 0.0)
        value = 0.0;
    return (pix_t)value;
}

/** ================================ PUBLIC ============================== **/
void 
vfwl_init_image(image_t *out) {
    out->w = 0;
    out->h = 0;
    out->bpp = 0;
    out->channels = NULL;
    out->nb_channels = 0;
}

void
vfwl_alloc_image(image_t *out, size_t w, size_t h, int nb_channels) {
    vfwl_free_image(out);
    vfwl_init_image(out);
    out->w = w;
    out->h = h;
    out->nb_channels = nb_channels;
    out->channels = _mm_malloc(nb_channels * w * h * sizeof(pix_t), 16);
}

void
vfwl_import_pnm(image_t *out, char *filename) {
    size_t tmp;
    FILE *p_file;
    unsigned int cols, rows, nb_channels;
    size_t c, r, ch, ch_size, l_size;
    unsigned char magic;
    int raw = 0;
    unsigned char *buffer;
    size_t offset = 0;
    pix_t * channels;
    unsigned int itmp;

    p_file = fopen(filename, "rb");
    if (p_file == NULL) {
        fprintf(stderr, "%s: Error while opening file\n", __FUNCTION__);
        return;
    }
    /* obtain file size: */
    fseek(p_file, 0, SEEK_END);
    l_size = ftell(p_file);
    rewind(p_file);
    /* allocate memory to contain the whole file: */
    buffer = (unsigned char*) malloc (sizeof(char)*l_size);
    if (buffer == NULL) {
        fprintf(stderr, "%s: Memory error\n", __FUNCTION__);
        fclose (p_file);
        return;
    }

    /* copy the file into the buffer: */
    tmp = fread (buffer, 1, l_size, p_file);
    if (tmp != l_size) {
        fprintf(stderr, "%s: Reading error, can't read all file bytes\n", __FUNCTION__);
        fclose (p_file);
        free (buffer);
        return;
    }
    /* close the opened file */
    fclose (p_file);

    /* Read whole file */
    magic = buffer[offset++];
    if (magic != 'P') {
        fprintf(stderr, "%s: Not a PNM file\n", __FUNCTION__);
        free (buffer);
        return;
    }
    magic = buffer[offset++];
    switch (magic) {
	case '1':	/* PBM ASCII */
	case '4':	/* PBM RAW */
        fprintf(stderr, "%s: PBM is not supported\n", __FUNCTION__);
        free (buffer);
	    return;
	case '5':	/* PGM RAW */
	    raw = 1;
	case '2':	/* PGM ASCII */
	    nb_channels = 1;
	    out->col_space = IT_CIEY;
	    break;
	case '6':	/* PPM RAW */
	    raw = 1;
	case '3':	/* PPM ASCII */
	    nb_channels = 3;
	    out->col_space = IT_RGB;
	    break;
	default:
        fprintf(stderr, "%s: Not a PNM file\n", __FUNCTION__);
        free (buffer);
	    return;
    }
    buffer = get_active_number(&cols, &buffer[offset]);
    buffer = get_active_number(&rows, buffer);
    buffer = get_active_number(&itmp, buffer);
    ch_size = cols*rows;
    vfwl_alloc_image(out, cols, rows, nb_channels);
    channels = out->channels;
    if (raw) {
        offset--;
        for (r = 0; r < rows; r++)
            for (c = 0; c < cols; c++)
                for (ch = 0; ch < nb_channels; ch++) {
                    channels[ch*ch_size+r*cols+c] = 
                            (pix_t)(buffer[offset++]);
                }
    } else {
        offset = 0;
        for (r = 0; r < rows; r++)
            for (c = 0; c < cols; c++)
                for (ch = 0; ch < nb_channels; ch++) {
                    buffer = get_active_number(&itmp, buffer);
                    channels[ch*ch_size+r*cols+c] = (pix_t)(itmp);
                }
    }
}

void
vfwl_export_pnm(char *filename, image_t *in, int raw) {
    FILE *p_file;
    unsigned char magic, d;
    pix_t val;
    unsigned int cols = in->w;
    unsigned int rows = in->h;
    unsigned int ch_size = cols*rows;
    unsigned int nb_channels = in->nb_channels;
    /* We assume that the header won't take more than 2048 bytes */
    char *buffer = (char*)malloc(nb_channels*cols*rows*sizeof(pix_t) + 2048);
    size_t len = 0, wlen;
    pix_t *channels = in->channels;
    size_t r, c, ch;
	unsigned int l = 70;

    switch (nb_channels) {
        case 1:
            magic = '2';
            break;

        case 3:
            magic = '3';
            break;

        default:
            fprintf(stderr, "%s: Unsupported number of channels %d\n", 
                    __FUNCTION__, nb_channels);
            return;
    }
    if (raw)
        magic += 3;

    sprintf(buffer, "P%c\n%d %d\n255", magic, cols, rows);
    len += strlen(buffer);
    if (raw) {
        buffer[len++] = '\n';
        for (r = 0; r < rows; r++)
            for (c = 0; c < cols; c++)
                for (ch = 0; ch < nb_channels; ch++) {
                    val = channels[ch*ch_size+r*cols+c];
                    if (val < 0)
                        d = 0;
                    else if (val > 255)
                        d = 255;
                    else
                        d = (unsigned char)val;
                    buffer[len++] = d;
                }
    } else {
	for (r = 0; r < rows; r++)
	    for (c = 0; c < cols; c++)
            for (ch = 0; ch < nb_channels; ch++) {
                val = channels[ch*ch_size+r*cols+c];
                if (val < 0)
                    d = 0;
                else if (val > 255)
                    d = 255;
                else
                    d = (unsigned char)val;
                l += 4;
                if (l >= 70) {
                    sprintf(&buffer[len], "\n%4d", (unsigned int)d);
                    len += strlen(&buffer[len]) + 1;
                    l = 3;
                } else {
                    sprintf(&buffer[len], " %4d", (unsigned int)d);
                    len += strlen(&buffer[len]) + 1;
                }
            }
        buffer[len++] = '\n';
    }
    p_file = fopen(filename, "wb");
    if (p_file == NULL) {
        fprintf(stderr, "%s: Error while opening file\n", __FUNCTION__);
        free (buffer);
        return;
    }

    wlen = fwrite(buffer, 1, len, p_file);
    if (wlen != len) {
        fprintf(stderr, "%s: Unable to write the whole buffer data\n", __FUNCTION__);
    }

    fclose(p_file);

    free(buffer);
}

void
vfwl_convert_RGB_to_YCbCr(image_t *in) {
    pix_t *channels[3];
    pix_t r, g, b;
    size_t k;
    size_t c_size = in->w * in->h;
    if (in->nb_channels != 3) {
        fprintf(stderr, "%s: The image is not RGB\n", __FUNCTION__);
        return;
    }
    /* Red component */
    channels[0] = &in->channels[0];
    /* Green component */
    channels[1] = &in->channels[c_size];
    /* Blue component */
    channels[2] = &in->channels[2*c_size];
    for(k = 0; k < c_size; k++) {
        r = *channels[0];
        g = *channels[1];
        b = *channels[2];
        /* Luminancy (Y) */
        (*(channels[0]++)) = (pix_t)(0.257 * r + 
                                     0.504 * g + 
                                     0.098 * b) + 16;
        /* Blue chrominancy (Cb) */
        (*(channels[1]++)) = (pix_t)(-0.148 * r - 
                                     0.291 * g + 
                                     0.439 * b) + 128;
        /* Red chrominancy (Cr) */
        (*(channels[2]++)) = (pix_t)(0.439 * r - 
                                     0.368 * g - 
                                     0.071 * b) + 128;
    }
}

void
vfwl_convert_YCbCr_to_RGB(image_t *in) {
    pix_t *channels[3];
    pix_t y, Cb, Cr;
    size_t k;
    size_t c_size = in->w * in->h;
    if (in->nb_channels != 3) {
        fprintf(stderr, "%s: The image is not RGB\n", __FUNCTION__);
        return;
    }
    /* Red component */
    channels[0] = &in->channels[0];
    /* Green component */
    channels[1] = &in->channels[c_size];
    /* Blue component */
    channels[2] = &in->channels[2*c_size];
    for(k = 0; k < c_size; k++) {
        y = *channels[0] - 16;
        Cb = *channels[1] - 128;
        Cr = *channels[2] - 128;
        /* Red */
        (*(channels[0]++)) = to_pixel(1.164*y - 0.002*Cb + 1.596*Cr);
        /* Green */
        (*(channels[1]++)) = to_pixel(1.164*y - 0.391*Cb - 0.814*Cr);
        /* Blue */
        (*(channels[2]++)) = to_pixel(1.164*y + 2.018*Cb - 0.001*Cr);
    }
}

void 
vfwl_free_image(image_t *in) {
    if (in->channels != NULL && in->nb_channels > 0) {
        _mm_free(in->channels);
    }
}
