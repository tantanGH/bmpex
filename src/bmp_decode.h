#ifndef __H_BMP_DECODE__
#define __H_BMP_DECODE__

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

typedef struct {
  int16_t brightness;
  int16_t half_size;
  int16_t extended_graphic;
  uint16_t* rgb555_r;
  uint16_t* rgb555_g;
  uint16_t* rgb555_b;
} BMP_DECODE_HANDLE;

int32_t bmp_decode_init(BMP_DECODE_HANDLE* bmp, int16_t brightness, int16_t half_size, int16_t extended_graphic);
void bmp_decode_close(BMP_DECODE_HANDLE* bmp);
int32_t bmp_decode_exec(BMP_DECODE_HANDLE* bmp, uint8_t* bmp_buffer, size_t bmp_buffer_bytes);

#endif