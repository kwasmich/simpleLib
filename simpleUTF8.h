//
//  simpleUTF8.h
//  utf8utils
//
//  Created by Michael Kwasnicki on 26.03.16.
//  Copyright © 2016 Michael Kwasnicki. All rights reserved.
//

#ifndef simpleUTF8_h
#define simpleUTF8_h


#include <stddef.h>



size_t utf8strlen(const char *s);
size_t utf8strnlen(const char *s, size_t maxlen);
size_t utf8wcslen(const wchar_t *s);
size_t utf8wcsnlen(const wchar_t *s, size_t maxlen);
void utf8wcstostr(char * const str, size_t maxlen, const wchar_t * const wcs);
void utf8strtowcs(wchar_t * const wcs, size_t maxlen, const char * const str);



#endif /* simpleUTF8_h */
