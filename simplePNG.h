#ifndef __PNG_H__
#define __PNG_H__


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool pngIsPNG(const uint8_t * const in_PNG_DATA);
bool pngDecode(uint8_t **out_image, uint32_t *out_width, uint32_t *out_height, uint32_t *out_numChannels, const uint8_t * const in_PNG_DATA, const size_t in_PNG_SIZE, const bool in_FLIP_Y);
bool pngEncode(uint8_t ** const out_pngData, size_t * out_pngSize, uint8_t * const in_IMAGE, const uint32_t in_WIDTH, const uint32_t in_HEIGHT, const uint32_t in_NUM_CHANNELS);
bool pngIsPNGFile(const char * const in_FILE_PATH);
bool pngRead(uint8_t ** const out_image, uint32_t * const out_width, uint32_t * const out_height, uint32_t * const out_numChannels, const char * const in_FILE_PATH, const bool in_FLIP_Y);
bool pngWrite(const char * const in_FILE_PATH, uint8_t * const in_IMAGE, const uint32_t in_WIDTH, const uint32_t in_HEIGHT, const uint32_t in_NUM_CHANNELS);
void pngFree(uint8_t ** const in_out_image);


#endif //__PNG_H__
