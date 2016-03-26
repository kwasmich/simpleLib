//
//  simpleUTF8.c
//  utf8utils
//
//  Created by Michael Kwasnicki on 26.03.16.
//  Copyright © 2016 Michael Kwasnicki. All rights reserved.
//

#include "simpleUTF8.h"

#include <assert.h>
#include <iso646.h>
#include <stdbool.h>



size_t utf8strlen(const char *s) {
    uint8_t c;
    size_t count = 0;
    size_t skip = 0;

    while (*s != '\0') {
        c = *s;

        if (skip > 0) {
            assert(((c & 0xC0) == 0x80) && "not a valid character");
            skip--;
        } else {
            if (c < 0x80) {         // Leave ASCII encoded
                skip = 0;
            } else if (c < 0xC0) {  // Not a valid character...
                assert(false && "not a valid character");
            } else if (c < 0xE0) {  // 110xxxxx 10xxxxxx
                skip = 1;
            } else if (c < 0xF0) {  // 11100xxx + 2
                skip = 2;
            } else if (c < 0xF8) {  // 111100xx + 3
                skip = 3;
            } else if (c < 0xFC) {  // 1111100x + 4
                skip = 4;
            } else if (c < 0xFE) {  // 1111110x + 5
                skip = 5;
            } else {                // Not a valid character...
                assert(false && "not a valid character");
            }

            count++;
        }

        s++;
    }

    return count;
}



size_t utf8strnlen(const char *s, size_t maxlen) {
    uint8_t c;
    size_t count = 0;
    size_t skip = 0;

    while ((*s != '\0') and (maxlen > 0)) {
        c = *s;

        if (skip > 0) {
            assert(((c & 0xC0) == 0x80) && "not a valid character");
            skip--;
        } else {
            if (c < 0x80) {         // Leave ASCII encoded
                skip = 0;
            } else if (c < 0xC0) {  // Not a valid character...
                assert(false && "not a valid character");
            } else if (c < 0xE0) {  // 110xxxxx 10xxxxxx
                skip = 1;
            } else if (c < 0xF0) {  // 11100xxx + 2
                skip = 2;
            } else if (c < 0xF8) {  // 111100xx + 3
                skip = 3;
            } else if (c < 0xFC) {  // 1111100x + 4
                skip = 4;
            } else if (c < 0xFE) {  // 1111110x + 5
                skip = 5;
            } else {                // Not a valid character...
                assert(false && "not a valid character");
            }

            count++;
        }

        s++;
        maxlen--;
    }

    return count;
}



size_t utf8wcslen(const wchar_t *s) {
    size_t count = 0;

    while (*s != '\0') {
        if (*s <= 0x7F) {               // Leave ASCII encoded
            count++;
        } else if (*s <= 0x07FF) {      // 110xxxxx 10xxxxxx
            count += 2;
        } else if (*s <= 0xFFFF) {      // 1110xxxx + 2
            count += 3;
        } else if (*s <= 0x1FFFFF) {    // 11110xxx + 3
            count += 4;
        } else if (*s <= 0x3FFFFFF) {   // 111110xx + 4
            count += 5;
        } else if (*s <= 0x7FFFFFFF) {  // 1111110x + 5
            count += 6;
        } else {                        // Not a valid character...
            assert(false && "not a valid character");
        }

        s++;
    }

    return count;
}



size_t utf8wcsnlen(const wchar_t *s, size_t maxlen) {
    size_t count = 0;

    while ((*s != L'\0') and (maxlen > 0)) {
        if (*s <= 0x7F) {               // Leave ASCII encoded
            count++;
        } else if (*s <= 0x07FF) {      // 110xxxxx 10xxxxxx
            count += 2;
        } else if (*s <= 0xFFFF) {      // 1110xxxx + 2
            count += 3;
        } else if (*s <= 0x1FFFFF) {    // 11110xxx + 3
            count += 4;
        } else if (*s <= 0x3FFFFFF) {   // 111110xx + 4
            count += 5;
        } else if (*s <= 0x7FFFFFFF) {  // 1111110x + 5
            count += 6;
        } else {                        // Not a valid character...
            assert(false && "not a valid character");
        }

        s++;
        maxlen--;
    }

    return count;
}



void utf8wcstostr(char * const str, size_t maxlen, const wchar_t * const wcs) {
    char *out = str;
    const wchar_t *in = wcs;
    wchar_t wc;

    while ((*in != L'\0') and (maxlen > 1)) {
        wc = *in;

        if ((wc <= 0x7F) and (maxlen > 1)) {         /* Leave ASCII encoded */
            out[0] = wc;
            out++;
            maxlen--;
        } else if ((wc <= 0x7FF) and (maxlen > 2)) {     /* 110xxxxx 10xxxxxx */
            out[0] = 0xC0 | (wc >> 6);
            out[1] = 0x80 | (wc & 0x3F);
            out += 2;
            maxlen -= 2;
        } else if ((wc <= 0xFFFF) and (maxlen > 3)) {     /* 1110xxxx + 2 */
            out[0] = 0xE0 | (wc >> 12);
            out[1] = 0x80 | ((wc >> 6) & 0x3F);
            out[2] = 0x80 | (wc & 0x3F);
            out += 3;
            maxlen -= 3;
        } else if ((wc <= 0x1FFFFF) and (maxlen > 4)) {    /* 11110xxx + 3 */
            out[0] = 0xF0 | (wc >> 18);
            out[1] = 0x80 | ((wc >> 12) & 0x3F);
            out[2] = 0x80 | ((wc >> 6) & 0x3F);
            out[3] = 0x80 | (wc & 0x3F);
            out += 4;
            maxlen -= 4;
        } else if ((wc <= 0x3FFFFFF) and (maxlen > 5)) {    /* 111110xx + 4 */
            out[0] = 0xF8 | (wc >> 24);
            out[1] = 0x80 | ((wc >> 18) & 0x3F);
            out[2] = 0x80 | ((wc >> 12) & 0x3F);
            out[3] = 0x80 | ((wc >> 6) & 0x3F);
            out[4] = 0x80 | (wc & 0x3F);
            out += 5;
            maxlen -= 5;
        } else if ((wc <= 0x7FFFFFFF) and (maxlen > 6)) {    /* 1111110x + 5 */
            out[0] = 0xFC | (wc >> 30);
            out[1] = 0x80 | ((wc >> 24) & 0x3F);
            out[2] = 0x80 | ((wc >> 18) & 0x3F);
            out[3] = 0x80 | ((wc >> 12) & 0x3F);
            out[4] = 0x80 | ((wc >> 6) & 0x3F);
            out[5] = 0x80 | (wc & 0x3F);
            out += 6;
            maxlen -= 6;
        } else {          /* Not a valid character... */
            break;
        }

        in++;
    }

    // zero terminating string;
    out[0] = L'\0';
}



void utf8strtowcs(wchar_t * const wcs, size_t maxlen, const char * const str) {
    wchar_t *out = wcs;
    const char *in = str;
    uint8_t c;
    size_t skip = 0;
    wchar_t composite = 0;

    while ((*in != '\0') and (maxlen > 1)) {
        c = *in;

        if (skip > 0) {
            assert(((c & 0xC0) == 0x80) && "not a valid character");
            composite = (composite << 6) | (c & 0x3F);
            skip--;
        } else {
            if (c < 0x80) {         // Leave ASCII encoded
                skip = 0;
                composite = c;
            } else if (c < 0xC0) {  // Not a valid character...
                assert(false && "not a valid character");
            } else if (c < 0xE0) {  // 110xxxxx 10xxxxxx
                skip = 1;
                composite = 0x1F & c;
            } else if (c < 0xF0) {  // 1110xxxx + 2
                skip = 2;
                composite = 0x0F & c;
            } else if (c < 0xF8) {  // 11110xxx + 3
                skip = 3;
                composite = 0x07 & c;
            } else if (c < 0xFC) {  // 111110xx + 4
                skip = 4;
                composite = 0x03 & c;
            } else if (c < 0xFE) {  // 1111110x + 5
                skip = 5;
                composite = 0x01 & c;
            } else {                // Not a valid character...
                assert(false && "not a valid character");
            }
        }

        if (skip == 0) {
            *out = composite;
            out++;
            maxlen--;
        }
        
        in++;
    }
    
    *out = L'\0';
}
