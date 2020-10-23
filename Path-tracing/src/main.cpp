#include <iostream>

#include <string>
#include <vector>
#include <unordered_map>

#include "includes/Main.hpp"
#include "includes/Vector3.hpp"
#include "includes/Objects.hpp"
#include "includes/Bitmap.hpp"
#include "includes/Ray-tracing.hpp"
#include "includes/Path-tracing.hpp"

const size_t width = 1920, height = 1080;
const int RAY_TRACING = 0;
const int PATH_TRACING = 1;
int trace_method;
int sceneId = 1;
size_t samplesPT = 256;
size_t threads = 4;
size_t step;

vector<Object*> objects;
vector<Pixel> image(width* height);

#if defined __linux__ || defined __APPLE__ 
pthread_t* s_threads;
#else
HANDLE* s_threads;
#endif

using namespace std;

#if defined __linux__ || defined __APPLE__ 
void* task(void* lpParam) {
#else
DWORD WINAPI task(LPVOID lpParam) {
#endif
    const size_t index = (size_t)lpParam;
    const size_t start_y = index * step;
    size_t end_y;
    if (index != threads - 1) {
        end_y = (index + 1) * step;
    }
    else {
        end_y = height;
    }

    float invWidth = 1 / float(width), invHeight = 1 / float(height);
    float fov = 60, aspectratio = width / float(height);
    float angle = tan(G_PI * 0.5 * fov / 180.);
    int idx = 0;
    Vec3f temp, illum;
    float dx, dy, xx, yy;
    // Trace rays
    for (size_t y = start_y; y < end_y; ++y) {
        //fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samplesPT * 4, 100. * y / (height - 1));
        for (size_t x = 0u; x < width; ++x) {
            idx = y * width + x;
            temp = Vec3f(0);
            if (RAY_TRACING == trace_method) {
                xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
                yy = (2 * ((y + 0.5) * invHeight) - 1) * angle;
                Vec3f raydir(xx, yy, -1);
                raydir.normalize();
                temp = ray_trace(Vec3f(0), raydir, objects, 0);
            }
            else if (PATH_TRACING == trace_method) {
                for (size_t sy = 0u; sy < 2u; ++sy) { // 2 subpixel row
                    for (std::size_t sx = 0u; sx < 2u; ++sx) {  // 2 subpixel columns
                        illum = Vec3f(0);
                        for (std::size_t s = 0u; s < samplesPT; ++s) { // samples per subpixel
                            const float r1 = 2.0 * genRandomFloat();
                            const float r2 = 2.0 * genRandomFloat();
                            dx = r1 < 1.0 ? sqrt(r1) - 1.0 : 1.0 - sqrt(2.0 - r1);
                            dy = r2 < 1.0 ? sqrt(r2) - 1.0 : 1.0 - sqrt(2.0 - r2);
                            xx = (2 * ((x + 0.5 * (dx + 0.5 + sx)) * invWidth) - 1) * angle * aspectratio;
                            yy = (2 * ((y + 0.5 * (dy + 0.5 + sy)) * invHeight) - 1) * angle;
                            Vec3f raydir(xx, yy, -1);
                            raydir.normalize();
                            Vec3f raypos = Vec3f(0);
                            illum += path_trace(raypos, raydir, objects);
                        }
                        temp += fclamp(illum / samplesPT) * 0.25f;
                    }
                }
            }
            temp = fclamp(temp);
            image[idx].r = (unsigned char)(temp.z * 255);
            image[idx].g = (unsigned char)(temp.y * 255);
            image[idx].b = (unsigned char)(temp.x * 255);
        }
    }
    return 0;
}


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

    string outFilePath = "pic.bmp";
    if(cmdLineParams.find("-out") != cmdLineParams.end())
        outFilePath = cmdLineParams["-out"];

    if(cmdLineParams.find("-scene") != cmdLineParams.end())
        sceneId = atoi(cmdLineParams["-scene"].c_str());

    if (cmdLineParams.find("-threads") != cmdLineParams.end())
        threads = atoi(cmdLineParams["-threads"].c_str());

    if (cmdLineParams.find("-samples") != cmdLineParams.end())
        samplesPT = atoi(cmdLineParams["-samples"].c_str());

    if (sceneId == 1) trace_method = RAY_TRACING;
    else if (sceneId == 2) trace_method = PATH_TRACING;
    else return EXIT_SUCCESS;


    // position, radius, surface color, reflectivity, transparency, emission color
    Plane plane_o = Plane(Vec3f(0, -3, 0), Vec3f(0, -1, 0), Vec3f(0.3), 0);
    BackSphere back_o = BackSphere(Vec3f(0, 0, -10), 15, Vec3f(0.7), 0);
    Sphere sphere_o = Sphere(Vec3f(-2, 0, -15), 3, Vec3f(0.30, 1, 0.30), 0.5);
    Box box_o = Box(Vec3f(2, 0, -10), 1, Vec3f(0.90, 0.2, 0.90), 0);
    Sphere light_o = Sphere(Vec3f(-4, 9, -4), 3, Vec3f(0), 0, Vec3f(2));
    if (RAY_TRACING == trace_method) {
        
    }
    else if (PATH_TRACING == trace_method) {
        light_o = Sphere(Vec3f(-4, 9, -4), 6, Vec3f(0), 0, Vec3f(10));
    }

    objects.push_back(&plane_o);
    objects.push_back(&back_o);
    objects.push_back(&sphere_o);
    objects.push_back(&box_o);
    // light
    objects.push_back(&light_o);
  
    step = height / threads;
    #if defined __linux__ || defined __APPLE__ 
        s_threads = new pthread_t[threads];
    #else
        s_threads = new HANDLE[threads];
    #endif
    for (size_t i = 0; i < threads; ++i) {
        #if defined __linux__ || defined __APPLE__ 
        pthread_create(&s_threads[i], NULL, task, (void*)i);
        #else
        s_threads[i] = CreateThread(nullptr, 0, task, reinterpret_cast<void*>(i), 0, nullptr);
        #endif
    }
    //waiting
    #if defined __linux__ || defined __APPLE__ 
    for (int i = 0; i < threads; ++i)
        pthread_join(s_threads[i], NULL);
    #else
        WaitForMultipleObjects(threads, s_threads, true, INFINITE);
    #endif
    

    SaveBMP(outFilePath.c_str(), &image[0], width, height);

    return EXIT_SUCCESS;
}