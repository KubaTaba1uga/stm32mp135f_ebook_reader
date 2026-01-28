#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
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

      /* uint16_t lum = (uint16_t)(r * 30 + g * 59 + b * 11) / 100; */

      const int threshold = 200;
      bool black = r < threshold || g < threshold || b < threshold;

      int byte_i = y * dst_stride + (x >> 3);

      int bit = 7 - (x & 7); // MSB first
      if (black) {
        dst[byte_i] |= (1u << bit);
      }
    }
  }
}

struct PixelARGB {
  uint8_t b, g, r, a;
};

void graphic_argb32_to_monochrome(uint8_t *dst, int w, int h,
                                  const uint8_t *src, int src_stride) {
  int dst_stride = w * sizeof(struct PixelARGB);
  memset(dst, 0xFF, dst_stride * h); // white

  for (int y = 0; y < h; y++) {
    const struct PixelARGB *src_row =
        (const struct PixelARGB *)(src + y * src_stride);
    struct PixelARGB *dst_row = (struct PixelARGB *)(dst + y * src_stride);

    for (int x = 0; x < w; x++) {
      const int threshold = 230;
      bool black =
          (src_row[x].r < threshold) || (src_row[x].g < threshold) || (src_row[x].b < threshold);

      uint8_t v = black ? 0x00 : 0xFF;
      dst_row[x] = (struct PixelARGB){.r=v, .g=v,.b=v, .a=0xFF};
    }
  }
}

/* struct Pixel { */
/*   uint8_t alpha; */
/*   uint8_t red; */
/*   uint8_t green; */
/*   uint8_t blue; */
/* }; */

/* void graphic_argb32_to_monochrome(uint8_t *dst, int w, int h, */
/*                                   const uint8_t *src, int stride) { */
/*   printf("sizeof(Pixel)=%lu\n", sizeof(struct Pixel)); */
/*   memset(dst, 0x00, w * sizeof(struct Pixel) * h); // 0 = white */

/*   for (int y = 0; y < h; y++) { */
/*     const struct Pixel *src_row = (const struct Pixel *)(src + y * stride);
 */
/*     struct Pixel *dst_row = (struct Pixel *)(dst + y * stride); */
/*     for (int x = 0; x < w; x++) { */
/*       struct Pixel pixel = src_row[x]; // 0xAARRGGBB on little-endian */

/*       /\* uint16_t lum = (uint16_t)(r * 30 + g * 59 + b * 11) / 100; *\/ */

/*       const int threshold = 210; */
/*       bool black = pixel.red < threshold || pixel.green < threshold || */
/*                    pixel.blue < threshold; */

/*       if (black) { */
/*         dst_row[x] = */
/*             (struct Pixel){.blue = 1, .green = 1, .red = 1, .alpha = 255}; */
/*       } */
/*     } */
/*   } */
/* } */
