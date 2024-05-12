#include <stdint.h>

int utf8_mbtowc16(uint16_t *pwc, const unsigned char *s) {
	unsigned char c = s[0];
	if (c < 0x80) {
		*pwc = c;
		return 1;
	} else if (c < 0xc2) {
		return -1;
	} else if (c < 0xe0) {
		if (!((s[1] ^ 0x80) < 0x40))
			return -1;
		*pwc = ((uint16_t)(c & 0x1f) << 6)
			| (uint16_t)(s[1] ^ 0x80);
		return 2;
	} else if (c < 0xf0) {
		if (!((s[1] ^ 0x80) < 0x40 && (s[2] ^ 0x80) < 0x40
			&& (c >= 0xe1 || s[1] >= 0xa0)))
			return -1;
		*pwc = ((uint16_t)(c & 0x0f) << 12)
			| ((uint16_t)(s[1] ^ 0x80) << 6)
			| (uint16_t)(s[2] ^ 0x80);
		return 3;
	} else
		return -1;
}