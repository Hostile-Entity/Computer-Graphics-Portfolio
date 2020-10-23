#include <string>
#include <vector>
#include <unordered_map>

#include "Bitmap.h"
#include "RayTracing.h"

using namespace std;

extern int ANTI_ALIASING;
extern int sceneId;

int main(int argc, const char** argv)
{
    unordered_map<string, string> cmdLineParams;

    for(int i=0; i<argc; i++)
    {
        string key(argv[i]);

    if(key.size() > 0 && key[0]=='-')
    {
        if(i != argc-1) // not last argument
        {
            cmdLineParams[key] = argv[i+1];
            i++;
        }
        else
            cmdLineParams[key] = "";
        }
    }

    string outFilePath = "zout.bmp";
    if(cmdLineParams.find("-out") != cmdLineParams.end())
        outFilePath = cmdLineParams["-out"];

    if(cmdLineParams.find("-scene") != cmdLineParams.end())
        sceneId = atoi(cmdLineParams["-scene"].c_str());

    if (sceneId == 3) ANTI_ALIASING = 3;

  
    vector<Pixel> image(width * height);
    int pixel = 0;
    float invWidth = 1 / float(width), invHeight = 1 / float(height);
    float fov = 30, aspectratio = width / float(height);
    float angle = tan(M_PI * 0.5 * fov / 180.);

    float3 temp;
    float2 alias = float2(0, 0);
    float xx, yy;
    // Trace rays
    for (int y = 0; y < height; ++y) {
        //fprintf(stderr, "\rRendering %5.2f%%", 100. * y / (height - 1));
        for (int x = 0; x < width; ++x, ++pixel) {
            temp = float3(0, 0, 0);
            for (int i = 0; i < ANTI_ALIASING; ++i) {
                for (int j = 0; j < ANTI_ALIASING; ++j) {
                    if (ANTI_ALIASING > 1) alias = 2.0 * float2(float(i), float(j)) / float(ANTI_ALIASING) - float2(1, 1);

                    xx = (2 * ((x + alias.x) * invWidth) - 1) * angle * aspectratio;
                    yy = (2 * ((y + alias.y) * invHeight) - 1) * angle;
                    float3 raydir(xx, yy, -1);
                    raydir = normalize(raydir);
                    temp += get_ray_color(float3(0, 0, 0), raydir);
                }
            }
            temp = temp / float(ANTI_ALIASING * ANTI_ALIASING);
            image[pixel].r = (unsigned char)(fmin(float(255), temp.z * 255));
            image[pixel].g = (unsigned char)(fmin(float(255), temp.y * 255));
            image[pixel].b = (unsigned char)(fmin(float(255), temp.x * 255));
        }
    }

    WriteBMP(outFilePath.c_str(), &image[0], width, height);

    return 0;
}