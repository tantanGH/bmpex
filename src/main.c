#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <doslib.h>
#include <iocslib.h>
#include "himem.h"
#include "crtc.h"
#include "bmp_decode.h"

#define VERSION "0.1.3 (2023/03/25)"

//
//  show help message
//
static void show_help_message() {
  printf("BMPEX.X - A simple BMP viewer for X680x0 " VERSION " by tantan\n");
  printf("usage: bmpex [options] <file.jpg>\n");
  printf("options:\n");
  printf("   -v<n> ... brightness(1-100, default:100)\n");
  printf("   -c    ... clear screen\n");
  printf("   -s    ... half size\n");
  printf("   -e    ... use XEiJ extended graphic (768x512x32768)\n");
  printf("   -h    ... show help message\n");
}

//
//  main
//
int32_t main(int32_t argc, uint8_t* argv[]) {

  // default return code
  int32_t rc = -1;

  // file name
  uint8_t* file_name = NULL;

  // buffer pointer
  uint8_t* file_data = NULL;

  // file pointer
  FILE* fp = NULL;

  // mode
  int16_t brightness = 100;
  int16_t clear_screen = 0;
  int16_t half_size = 0;
  int16_t extended_graphic = 0;

  // check command line
  if (argc < 2) {
    show_help_message();
    goto exit;
  }

  // parse command lines
  for (int16_t i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && strlen(argv[i]) >= 2) {
      if (argv[i][1] == 'v') {
        brightness = atoi(argv[i]+2);
        if (brightness < 1 || brightness > 100) {
          show_help_message();
          goto exit;
        }
      } else if (argv[i][1] == 'c') {
        clear_screen = 1;
      } else if (argv[i][1] == 's') {
        half_size = 1;
      } else if (argv[i][1] == 'e') {
        extended_graphic = 1;
      } else if (argv[i][1] == 'h') {
        show_help_message();
        goto exit;
      } else {
        printf("error: unknown option (%s).\n",argv[i]);
        goto exit;
      }
    } else {
      if (file_name != NULL) {
        printf("error: too many files.\n");
        goto exit;
      }
      file_name = argv[i];
    }
  }

  if (file_name == NULL) {
    show_help_message();
    goto exit;
  }
  
  fp = fopen(file_name, "rb");
  if (fp == NULL) {
    printf("error: failed to open file: %s\n", file_name);
    goto exit;
  }

  fseek(fp, 0, SEEK_END);
  size_t file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  file_data = (uint8_t*)himem_malloc(file_size, 0);
  if (file_data == NULL) {
    printf("error: out of memory.\n");
    goto exit;
  }

  size_t read_len = 0;
  do {
    size_t len = fread(file_data + read_len, 1, file_size - read_len, fp);
    if (len == 0) break;
    read_len += len;
  } while (read_len < file_size);
  fclose(fp);
  fp = NULL;

  B_SUPER(0);
  C_CUROFF();

  if (clear_screen) {
    C_CLS_AL();
    if (!extended_graphic) {
      CRTMOD(16);
      G_CLR_ON();
    }
  }

  crtc_set_extra_mode(extended_graphic);
  if (extended_graphic) {
    if (clear_screen) {
      struct FILLPTR fillptr = { 0, 0, 767, 511, 0 };   // cannot use G_CLR_ON
      FILL(&fillptr);
    }
  }

  BMP_DECODE_HANDLE bmp_decode = { 0 };
  bmp_decode_init(&bmp_decode, brightness, half_size, extended_graphic);

  rc = bmp_decode_exec(&bmp_decode, file_data, file_size);
  if (rc != 0) {
    printf("error: BMP decode error. (rc=%d)\n", rc);
  }
  bmp_decode_close(&bmp_decode);

exit:

  C_CURON();

  if (fp != NULL) {
    fclose(fp);
    fp = NULL;
  }

  if (file_data != NULL) {
    himem_free(file_data, 0);
    file_data = NULL;
  }

  return rc;
}