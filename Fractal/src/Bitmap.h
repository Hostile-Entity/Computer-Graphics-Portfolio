#ifndef BITMAP_GUARDIAN_H
#define BITMAP_GUARDIAN_H

#include <algorithm>


struct Pixel { unsigned char r, g, b; };

void WriteBMP(const char* fname, Pixel* a_pixelData, int width, int height);

#endif 
