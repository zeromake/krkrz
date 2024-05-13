#include <string>
#include "uni_utf8.h"

bool IsUTF8LeadByte(tjs_nchar b) {
    return (b & 0xC0) != 0x80;
}

tjs_uint UnicodeToUTF8(tjs_char in) {
    if (in < 0x80) return in;
    if (in < 0x800) return 0xC0 | (in >> 6) | ((in & 0x3F) << 8);
    return 0xE0 | (in >> 12) | ((in >> 6 & 0x3F) << 8) | ((in & 0x3F) << 16);
}
tjs_size UnicodeToUTF8String(const tjs_char *in, tjs_nchar* out) {
    tjs_size i = 0;
    while (in[i]) {
        tjs_uint utf8 = UnicodeToUTF8(in[i]);
        if (utf8 < 0x100) {
            out[i] = utf8;
        } else if (utf8 < 0x10000) {
            out[i] = utf8 & 0xFF;
            out[i + 1] = utf8 >> 8;
        } else {
            out[i] = utf8 & 0xFF;
            out[i + 1] = utf8 >> 8 & 0xFF;
            out[i + 2] = utf8 >> 16;
        }
        i++;
    }
    return i;

}
tjs_size UnicodeToUTF8String(const tjs_char *in, tjs_nchar* out, tjs_size limit) {
    tjs_size i = 0;
    while (in[i] && i < limit) {
        tjs_uint utf8 = UnicodeToUTF8(in[i]);
        if (utf8 < 0x100) {
            out[i] = utf8;
        } else if (utf8 < 0x10000) {
            out[i] = utf8 & 0xFF;
            out[i + 1] = utf8 >> 8;
        } else {
            out[i] = utf8 & 0xFF;
            out[i + 1] = utf8 >> 8 & 0xFF;
            out[i + 2] = utf8 >> 16;
        }
        i++;
    }
    return i;
}

tjs_size UTF8ToUnicodeString(const char * in, tjs_char *out) {
    tjs_size i = 0;
    while (in[i]) {
        if (in[i] < 0x80) {
            out[i] = in[i];
        } else if (in[i] < 0xE0) {
            out[i] = in[i] & 0x3F;
            out[i] |= (in[i + 1] & 0x3F) << 6;
        } else {
            out[i] = in[i] & 0x1F;
            out[i] |= (in[i + 1] & 0x3F) << 5;
            out[i] |= (in[i + 2] & 0x3F) << 11;
        }
        i++;
    }
    return i;

}
tjs_size UTF8ToUnicodeString(const char * in, tjs_char *out, tjs_size limit ) {
    tjs_size i = 0;
    while (in[i] && i < limit) {
        if (in[i] < 0x80) {
            out[i] = in[i];
        } else if (in[i] < 0xE0) {
            out[i] = in[i] & 0x3F;
            out[i] |= (in[i + 1] & 0x3F) << 6;
        } else {
            out[i] = in[i] & 0x1F;
            out[i] |= (in[i + 1] & 0x3F) << 5;
            out[i] |= (in[i + 2] & 0x3F) << 11;
        }
        i++;
    }
    return i;
}