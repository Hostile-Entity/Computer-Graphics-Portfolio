#include <vector>
#include <fstream>
#include <cstring>

#include "Bitmap.h"

void WriteBMP(const char* fname, Pixel* a_pixelData, int width, int height)
{
    int paddedsize = (width * height) * sizeof(Pixel);

    unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
    unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };

    bmpfileheader[2] = (unsigned char)(paddedsize);
    bmpfileheader[3] = (unsigned char)(paddedsize >> 8);
    bmpfileheader[4] = (unsigned char)(paddedsize >> 16);
    bmpfileheader[5] = (unsigned char)(paddedsize >> 24);

    bmpinfoheader[4] = (unsigned char)(width);
    bmpinfoheader[5] = (unsigned char)(width >> 8);
    bmpinfoheader[6] = (unsigned char)(width >> 16);
    bmpinfoheader[7] = (unsigned char)(width >> 24);
    bmpinfoheader[8] = (unsigned char)(height);
    bmpinfoheader[9] = (unsigned char)(height >> 8);
    bmpinfoheader[10] = (unsigned char)(height >> 16);
    bmpinfoheader[11] = (unsigned char)(height >> 24);


    std::ofstream out(fname, std::ios::out | std::ios::binary);
    out.write((const char*)bmpfileheader, 14);
    out.write((const char*)bmpinfoheader, 40);
    out.write((const char*)a_pixelData, paddedsize);
    out.flush();
    out.close();
}