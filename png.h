#ifndef PNG_H
#define PNG_H

#include "libraries/lv_lib_png/lv_png.h"
bool loadPNG(const char* file, int w, int h, uint8_t* data, lv_img_dsc_t* img);
bool downloadPNG(const char* url, const char* filename, int width, int height, uint8_t* data, lv_img_dsc_t* img);
bool downloadPNG(const char* url, const char* filename, int width, int height, uint8_t* data, lv_img_dsc_t* img, int* progress, int total, void (* onProgress)(int progress, int total));

#endif
