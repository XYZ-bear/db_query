#pragma once

#include <vector>
#include <map>
#include <string>
using namespace std;

#define   MAX_VARINT32_BYTES   5
#define   MAX_UINT8            256
#define   MAX_UINT16           65536
#define   MAX_UINT24           16777216
#define   MAX_UINT32           4294967296

size_t varint_pack(uint32_t value, uint8_t *out)
{
	unsigned rv = 0;

	if (value >= 0x80) {
		out[rv++] = value | 0x80;
		value >>= 7;
		if (value >= 0x80) {
			out[rv++] = value | 0x80;
			value >>= 7;
			if (value >= 0x80) {
				out[rv++] = value | 0x80;
				value >>= 7;
				if (value >= 0x80) {
					out[rv++] = value | 0x80;
					value >>= 7;
				}
			}
		}
	}
	/* assert: value<128 */
	out[rv++] = value;
	return rv;
}

size_t varint_pack(uint64_t value, uint8_t *out)
{
	uint32_t hi = (uint32_t)(value >> 32);
	uint32_t lo = (uint32_t)value;
	unsigned rv;

	if (hi == 0)
		return varint_pack((uint32_t)lo, out);
	out[0] = (lo) | 0x80;
	out[1] = (lo >> 7) | 0x80;
	out[2] = (lo >> 14) | 0x80;
	out[3] = (lo >> 21) | 0x80;
	if (hi < 8) {
		out[4] = (hi << 4) | (lo >> 28);
		return 5;
	}
	else {
		out[4] = ((hi & 7) << 4) | (lo >> 28) | 0x80;
		hi >>= 3;
	}
	rv = 5;
	while (hi >= 128) {
		out[rv++] = hi | 0x80;
		hi >>= 7;
	}
	out[rv++] = hi;
	return rv;
}

size_t varint_size(uint32_t v)
{
	if (v < (1 << 7)) {
		return 1;
	}
	else if (v < (1 << 14)) {
		return 2;
	}
	else if (v < (1 << 21)) {
		return 3;
	}
	else if (v < (1 << 28)) {
		return 4;
	}
	else {
		return 5;
	}
}

size_t varint_size(uint64_t v)
{
	uint32_t upper_v = (uint32_t)(v >> 32);

	if (upper_v == 0) {
		return varint_size((uint32_t)v);
	}
	else if (upper_v < (1 << 3)) {
		return 5;
	}
	else if (upper_v < (1 << 10)) {
		return 6;
	}
	else if (upper_v < (1 << 17)) {
		return 7;
	}
	else if (upper_v < (1 << 24)) {
		return 8;
	}
	else if (upper_v < (1U << 31)) {
		return 9;
	}
	else {
		return 10;
	}
}

size_t varint_unpack(const uint8_t *&buffer, uint32_t * value)
{
	const uint8_t * ptr = buffer;

	uint32_t  i = 0;
	uint32_t  b;
	uint32_t  result;

	b = *(ptr++); result = (b & 0x7F); if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= (b & 0x7F) << 7; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= (b & 0x7F) << 14; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= (b & 0x7F) << 21; if (!(b & 0x80)) goto done;
	b = *(ptr++); result |= b << 28; if (!(b & 0x80)) goto done;

	for (; i < MAX_VARINT32_BYTES; i++)
	{
		b = *(ptr++); if (!(b & 0x80)) goto done;
	}

	return NULL;
done:
	*value = result;
	return ptr - buffer;
}

size_t varint_unpack(const uint8_t *buf, uint64_t * value)
{
	int      shift, n;
	uint64_t x, c;

	n = 0;
	x = 0;

	for (shift = 0; shift < 64; shift += 7) {
		c = (uint64_t)buf[n++];
		x |= (c & 0x7F) << shift;
		if ((c & 0x80) == 0) {
			break;
		}
	}
	*value = x;
	return n;
}

size_t varint_scan(const uint8_t *buf)
{
	int      shift, n;
	uint64_t x, c;
	n = 0;
	x = 0;
	for (shift = 0; shift < 64; shift += 7) {
		c = (uint64_t)buf[n++];
		if ((c & 0x80) == 0) {
			break;
		}
	}
	return n;
}


inline uint32_t zigZag_encode(int32_t  n) { return (n << 1) ^ (n >> 31); }
inline uint64_t zigZag_encode(int64_t  n) { return (n << 1) ^ (n >> 63); }
inline int32_t  zigZag_decode(uint32_t n) { return (n >> 1) ^ -(int32_t)(n & 1); }
inline int64_t  zigZag_decode(uint64_t n) { return (n >> 1) ^ -(int64_t)(n & 1); }