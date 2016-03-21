//
//  PNG.c
//  PNG
//
//  Created by Michael Kwasnicki on 19.03.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "simplePNG.h"

#include <png.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>



struct mem_decode {
    const uint8_t *buffer;
    size_t size;
};


struct mem_encode {
    uint8_t *buffer;
    size_t size;
};



static void png_read_data_fn(png_structp png_ptr, png_bytep out_data, png_size_t length) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    struct mem_decode *state = (struct mem_decode *)io_ptr;
    memcpy(out_data, state->buffer, length);
    state->buffer += length;
    assert(state->size >= length);
    state->size -= length;
}



static void png_write_data_fn(png_structp png_ptr, png_bytep data, png_size_t length) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    struct mem_encode *state = (struct mem_encode *)io_ptr;
    size_t newSize = state->size + length;

    if (state->buffer) {
        void *newBuffer = realloc(state->buffer, newSize);

        if (!newBuffer) {
            free(state->buffer);
        }

        state->buffer = newBuffer;
    } else {
        state->buffer = malloc(newSize);
    }

    if(!state->buffer) {
        png_error(png_ptr, "Write Error");
    }

    memcpy(state->buffer + state->size, data, length);
    state->size += length;
}



bool pngIsPNG(const uint8_t * const in_PNG_DATA) {
    int result = png_sig_cmp(in_PNG_DATA, 0, 8);
    return result == 0;
}



bool pngDecode(uint8_t **out_image, uint32_t *out_width, uint32_t *out_height, uint32_t *out_numChannels, const uint8_t * const in_PNG_DATA, const size_t in_PNG_SIZE, const bool in_FLIP_Y) {
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (png_ptr == NULL) {
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (info_ptr == NULL) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }

    struct mem_decode state = (struct mem_decode){
        .buffer = in_PNG_DATA,
        .size = in_PNG_SIZE
    };

    png_set_read_fn(png_ptr, &state, png_read_data_fn);
    png_read_info(png_ptr, info_ptr);
    png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    uint32_t numChannels;

    switch (color_type) {
        case PNG_COLOR_TYPE_GRAY:
            numChannels = 1;

            if (bit_depth < 8) {
                png_set_packing(png_ptr);
                png_set_expand_gray_1_2_4_to_8(png_ptr);
            }

            break;

        case PNG_COLOR_TYPE_GRAY_ALPHA:
            numChannels = 2;
            break;

        case PNG_COLOR_TYPE_PALETTE:
            numChannels = 3;
            png_set_packing(png_ptr);
            png_set_palette_to_rgb(png_ptr);
            break;

        case PNG_COLOR_TYPE_RGB:
            numChannels = 3;
            break;

        case PNG_COLOR_TYPE_RGB_ALPHA:
            numChannels = 4;
            break;

        default:
            numChannels = 1;
            break;
    }

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) != 0) {
        png_set_tRNS_to_alpha(png_ptr);
    }

    png_set_scale_16(png_ptr);
    png_read_update_info(png_ptr, info_ptr);
    uint8_t *image = malloc(width * height * numChannels * sizeof(png_byte));
    png_byte **png_rows = malloc(height * sizeof(png_byte *));

    for (size_t row = 0; row < height; row++) {
        if (in_FLIP_Y) {
            png_rows[height - 1 - row] = &image[row * numChannels * width];
        } else {
            png_rows[row] = &image[row * numChannels * width];
        }
    }

    png_read_image(png_ptr, png_rows);
    free(png_rows);
    *out_image = image;
    *out_width = width;
    *out_height = height;
    *out_numChannels = numChannels;
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return true;
}



bool pngEncode(uint8_t ** const out_pngData, size_t * out_pngSize, uint8_t * const in_IMAGE, const uint32_t in_WIDTH, const uint32_t in_HEIGHT, const uint32_t in_NUM_CHANNELS) {
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (png_ptr == NULL) {
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (info_ptr == NULL) {
        png_destroy_write_struct(&png_ptr, NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }

    png_bytep *row_pointers = malloc(in_HEIGHT * sizeof(png_bytep));
    size_t bytes_per_pixel = in_NUM_CHANNELS * sizeof(png_byte);

    for (size_t k = 0; k < in_HEIGHT; k++) {
        row_pointers[k] = &in_IMAGE[k * in_WIDTH * bytes_per_pixel];
    }

    png_byte color_type = PNG_COLOR_TYPE_GRAY;
    color_type = (in_NUM_CHANNELS == 2) ? PNG_COLOR_TYPE_GRAY_ALPHA : color_type;
    color_type = (in_NUM_CHANNELS == 3) ? PNG_COLOR_TYPE_RGB : color_type;
    color_type = (in_NUM_CHANNELS == 4) ? PNG_COLOR_TYPE_RGB_ALPHA : color_type;

    struct mem_encode state = (struct mem_encode){
        .buffer = NULL,
        .size = 0
    };

    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_set_IHDR(png_ptr, info_ptr, in_WIDTH, in_HEIGHT, 8, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_BASE);
    png_set_write_fn(png_ptr, &state, png_write_data_fn, NULL);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    free(row_pointers);
    *out_pngData = state.buffer;
    *out_pngSize = state.size;
    return true;
}



bool pngIsPNGFile(const char * const in_FILE_PATH) {
    png_byte buf[8];
    FILE *fp = fopen(in_FILE_PATH, "rb");

    if (!fp) {
        fprintf(stderr, "Cannot open file \"%s\"\n", in_FILE_PATH);
        return false;
    }

    size_t bytes_read = fread(buf, 1, 8, fp);

    if (bytes_read != 8) {
        fprintf(stderr, "Cannot read file \"%s\"\n", in_FILE_PATH);
        fclose(fp);
        return false;
    }

    fclose(fp);
    return pngIsPNG(buf);
}



bool pngRead(uint8_t ** const out_image, uint32_t * const out_width, uint32_t * const out_height, uint32_t * const out_numChannels, const char * const in_FILE_PATH, const bool in_FLIP_Y) {
    FILE * fp = fopen(in_FILE_PATH, "rb");

    if (fp == NULL) {
        fprintf(stderr, "Cannot open file \"%s\"\n", in_FILE_PATH);
        return false;
    }

    struct stat st;
    int fd = fileno(fp);
    int ret = fstat(fd, &st);

    if (ret != 0) {
        fprintf(stderr, "Cannot get file stats for \"%s\"\n", in_FILE_PATH);
        fclose(fp);
        return false;
    }

    size_t len = st.st_size;

    if (len == 0) {
        fprintf(stderr, "File \"%s\" is empty\n", in_FILE_PATH);
        fclose(fp);
        return false;
    }

    uint8_t *rawData = malloc(len);
    size_t bytes_read = fread(rawData, sizeof(uint8_t), len, fp);

    if (bytes_read != len) {
        fprintf(stderr, "Cannot read file \"%s\"\n", in_FILE_PATH);
        free(rawData);
        fclose(fp);
        return false;
    }

    fclose(fp);
    bool success = pngDecode(out_image, out_width, out_height, out_numChannels, rawData, len, in_FLIP_Y);
    free(rawData);
    return success;
}



bool pngWrite(const char * const in_FILE_PATH, uint8_t * const in_IMAGE, const uint32_t in_WIDTH, const uint32_t in_HEIGHT, const uint32_t in_NUM_CHANNELS) {
    uint8_t *data;
    size_t size;
    bool success = pngEncode(&data, &size, in_IMAGE, in_WIDTH, in_HEIGHT, in_NUM_CHANNELS);

    if (success) {
        FILE *fp = fopen(in_FILE_PATH, "wb");

        if (fp == NULL) {
            fprintf(stderr, "Cannot open file \"%s\"\n", in_FILE_PATH);
            pngFree(&data);
            return false;
        }

        size_t bytes_written = fwrite(data, sizeof(uint8_t), size, fp);

        if (bytes_written != size) {
            fprintf(stderr, "Cannot write file \"%s\"\n", in_FILE_PATH);
            fclose(fp);
            pngFree(&data);
            return false;
        }

        fclose(fp);
    }

    return success;
}



void pngFree(uint8_t ** const in_out_image) {
    free(*in_out_image);
    *in_out_image = NULL;
}
