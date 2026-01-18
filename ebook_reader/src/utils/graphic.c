#include <stdbool.h>
#include <stdint.h>
#include <string.h>

void graphic_argb32_to_i1(uint8_t *dst, int w, int h, const uint8_t *src,
                          int stride) {
  int dst_stride = (w + 7) / 8;
  memset(dst, 0x00, dst_stride * h); // 0 = white

  for (int y = 0; y < h; y++) {
    const uint32_t *row = (const uint32_t *)(src + y * stride);
    for (int x = 0; x < w; x++) {
      uint32_t p = row[x]; // 0xAARRGGBB on little-endian
      uint8_t r = (p >> 16) & 0xFF;
      uint8_t g = (p >> 8) & 0xFF;
      uint8_t b = (p >> 0) & 0xFF;

      // luminance (cheap)
      uint16_t lum = (uint16_t)(r * 30 + g * 59 + b * 11) / 100;

      // choose what looks good for e-ink
      bool black = lum > 130;

      int byte_i = y * dst_stride + (x >> 3);
      int bit = 7 - (x & 7); // MSB first
      if (black) {
        dst[byte_i] |= (1u << bit);
      }
    }
  }
}

void graphic_argb32_to_a1(uint8_t *dst, int w, int h, const uint8_t *src,
                          int stride) {
  int dst_stride = (w + 7) / 8;
  memset(dst, 0x00, dst_stride * h); // 0 = black (you can invert if needed)

  for (int y = 0; y < h; y++) {
    const uint32_t *row = (const uint32_t *)(src + y * stride);

    for (int x = 0; x < w; x++) {
      uint32_t p = row[x]; // 0xAARRGGBB (little-endian)
      uint8_t a = (p >> 24) & 0xFF;
      uint8_t r = (p >> 16) & 0xFF;
      uint8_t g = (p >> 8) & 0xFF;
      uint8_t b = (p >> 0) & 0xFF;

      uint16_t lum = (uint16_t)(r * 30 + g * 59 + b * 11) / 100;
      bool is_ink = (a > 0) && (lum < 130); 

      int byte_i = y * dst_stride + (x >> 3);
      int bit = 7 - (x & 7); // MSB first

      if (is_ink)
        dst[byte_i] |= (1u << bit); // 1 = opaque
    }
  }
}
