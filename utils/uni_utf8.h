#ifndef _UNI_UTF8_H_
#define _UNI_UTF8_H_
#include "tjsTypes.h"

//---------------------------------------------------------------------------
bool IsUTF8LeadByte(tjs_nchar b);
tjs_uint UnicodeToUTF8(tjs_char in);
tjs_size UnicodeToUTF8String(const tjs_char *in, tjs_nchar* out);
tjs_size UnicodeToUTF8String(const tjs_char *in, tjs_nchar* out, tjs_size limit);
tjs_size UTF8ToUnicodeString(const char * in, tjs_char *out);
tjs_size UTF8ToUnicodeString(const char * in, tjs_char *out, tjs_size limit);
//---------------------------------------------------------------------------
#endif
