#ifndef __PNG_H__
#define __PNG_H__


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


typedef enum {
    kPNG8 = 8,
    kPNG16 = 16
} pngBitDepth;


typedef enum {
    kPNG_L = 1,
    kPNG_LA = 2,
    kPNG_RGB = 3,
    kPNG_RGBA = 4
} pngChannels;


bool pngIsPNG(const uint8_t * const in_PNG_DATA);
bool pngDecode(uint8_t ** const out_image, uint32_t * const out_width, uint32_t * const out_height, pngChannels * const out_numChannels, pngBitDepth * const out_bitDepth, const uint8_t * const in_PNG_DATA, const size_t in_PNG_SIZE, const bool in_FLIP_Y);
bool pngEncode(uint8_t ** const out_pngData, size_t * const out_pngSize, uint8_t * const in_IMAGE, const uint32_t in_WIDTH, const uint32_t in_HEIGHT, const pngChannels in_NUM_CHANNELS, const pngBitDepth in_BIT_DEPTH);
bool pngIsPNGFile(const char * const in_FILE_PATH);
bool pngRead(uint8_t ** const out_image, uint32_t * const out_width, uint32_t * const out_height, pngChannels * const out_numChannels, pngBitDepth * const out_bitDepth, const char * const in_FILE_PATH, const bool in_FLIP_Y);
bool pngWrite(const char * const in_FILE_PATH, uint8_t * const in_IMAGE, const uint32_t in_WIDTH, const uint32_t in_HEIGHT, const pngChannels in_NUM_CHANNELS, const pngBitDepth in_BIT_DEPTH);
void pngFree(uint8_t ** const in_out_image);


#endif //__PNG_H__
