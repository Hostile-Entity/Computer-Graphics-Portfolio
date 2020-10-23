#include <vector>
#include <fstream>
#include <cstring>

#include "RayTracing.h"

using namespace LiteMath;

int ANTI_ALIASING = 1;
int sceneId = 1;

float get_reflect_fraction(const float& material)
{
    if (material < 1.5) return 0.0; //plane
    if (material < 2.5) return 1; //sphere
    if (material < 3.5) return 0; //box
    if (material < 4.5) return 0.0; //cone
    return 0;
}

float3 get_diffuse_color(const float& material)
{
    if (material < 1.5) return float3(0.0, 0.2, 1.0); //plane
    if (material < 2.5) return float3(0.22, 0.0, 0.39); //sphere
    if (material < 3.5) return float3(0.5, 0.5, 0.5); //box
    if (material < 4.5) return float3(0.41, 0.8, 1.0); //cone
    return float3(1.0, 0.0, 0.0);
}

float inline sdPlane(const float3& p)
{
    return p.y;
}

float inline sdSphere(const float3& p, const float& s)
{
    return length(p) - s;
}

float inline sdBox(const float3& p, const float3& b)
{
    float3 d = float3(fabs(p.x), fabs(p.y), fabs(p.z)) - b;
    return fmin(maxcomp(d), 0.0) + length(float3(fmax(d.x, 0.0), fmax(d.y, 0.0), fmax(d.z, 0.0)));
}

float inline sdVerticalCapsule(float3 p, const float& h, const float& r)
{
    p.y -= clamp(p.y, 0.0, h);
    return length(p) - r;
}

float inline sdCone(const float3& p, const float3& c)
{
    float2 q = float2(length(float2(p.x, p.z)), p.y);
    float d1 = -q.y - c.z;
    float d2 = fmax(dot(q, float2(c.x, c.y)), q.y);
    return length(float3(fmax(d1, 0), fmax(d2, 0), 0.0)) + fmin(fmax(d1, d2), 0.);
}

float inline sdOctahedron(const float3& p, const float& s)
{
    return (fabs(p.x) + fabs(p.y) + fabs(p.z) - s) * 0.57735027;
}

float sd_mandelbulb(const float3& p) {
    float3 p_inv = float3(p.x, p.z, p.y);
    float3 z = p_inv;
    float3 dz = float3(0, 0, 0);
    float power = 8.0;
    float r, theta, phi;
    float dr = 1.0;

    for (int i = 0; i < 7; ++i) {
        r = length(z);
        if (r > 2.0) continue;
        theta = atan(z.y / z.x);

        phi = asin(z.z / r);

        dr = pow(r, power - 1.0) * dr * power + 1.0;

        r = pow(r, power);
        theta = theta * power;
        phi = phi * power;

        z = r * float3(cos(theta) * cos(phi), sin(theta) * cos(phi), sin(phi)) + p_inv;

    }
    return 0.5 * log(r) * r / dr;
}

float2 inline opU(const float2& d1, const float2& d2)
{
    return (d1.x < d2.x) ? d1 : d2;
}

float2 sd_scene(const float3& pos)
{
    float2 res;

    if (sceneId == 1) {
        res = float2(sdPlane(pos - float3(0, -3, 0)), 1.0);

        res = opU(res, float2(sdSphere(pos - float3(0.45, 0.25, -10.0), 0.25), 2.0));

        res = opU(res, float2(sdVerticalCapsule(pos - float3(1.25, -0.25, -10.0), 1.25, 0.25), 3.0));

        res = opU(res, float2(sdOctahedron(pos - float3(-1.0, 0.5, -10.0), 0.3), 4.0));
    }
    else if (sceneId == 2) {
        res = float2(sdPlane(pos - float3(0, -3, 0)), 1.0);
        res = opU(res, float2(sd_mandelbulb(pos - float3(0, 0, -7.0)), 3.0));
    }
    else if (sceneId == 3) {
        res = float2(sdSphere(pos - float3(0, 0.25, -3.0), 0.25), 1.0);
        res = opU(res, float2(sdSphere(pos - float3(1, 1, -5.0), 0.25), 1.0));
        res = opU(res, float2(sdSphere(pos - float3(-1, -1, -5.0), 0.25), 1.0));
    }
    return res;
}

float3 inline reflect(const float3& ray, const float3& norm) {
    return ray - 2 * dot(ray, norm) * norm;
}

float2 ray_march(const float3& ray_pos, const float3& ray_dir)
{
    float eps = 0.0005;
    float tmin = eps;
    float tmax = 20.0;

    float t = tmin;
    float m = -1.0;
    int i;
    for (i = 0; i < 50; i++)
    {
        float precis = eps * t;
        float2 res = sd_scene(ray_pos + ray_dir * t);
        if (res.x < precis || t > tmax) break;
        t += res.x;
        m = res.y;
    }

    if (t > tmax) m = -1.0;
    return float2(t, m);
}

bool doesFiniteRayHitAnything(const float3& ray_pos, const float3& ray_dir, const float& length)
{
    float2 hit = ray_march(ray_pos, ray_dir);
    if (hit.y < 0 || hit.x > length) {
        return false;
    }
    return true;
}

float3 estimate_normal(const float3& pos)
{
    float eps = 0.0005;
    float3 norm = float3(
        sd_scene(pos + float3(eps, 0.0, 0.0)).x - sd_scene(pos - float3(eps, 0.0, 0.0)).x,
        sd_scene(pos + float3(0.0, eps, 0.0)).x - sd_scene(pos - float3(0.0, eps, 0.0)).x,
        sd_scene(pos + float3(0.0, 0.0, eps)).x - sd_scene(pos - float3(0.0, 0.0, eps)).x);
    return normalize(norm);
}

float3 light_in_pos(const float3& intersect_point, const float3& nor, const float3& ref, const float& material)
{
    float3 light_pos = float3(-5, 7, 0);

    float3 color = float3(0, 0, 0);

    float3 light_illum = float3(0, 0, 0);
    float3 light_to_intersect = intersect_point - light_pos;
    float dot_r = -dot(light_to_intersect, nor);
    if (dot_r < 0.0f) return color;

    float light_dist = length(light_to_intersect);
    light_to_intersect = normalize(light_to_intersect);
    dot_r = dot_r / light_dist;

    if (doesFiniteRayHitAnything(intersect_point, -1.0 * light_to_intersect, light_dist)) return color;
    float diffuse_intensity = dot_r / (light_dist * light_dist);

    light_illum += get_diffuse_color(material) * diffuse_intensity;

    float specular_coeff = 10.0f;
    float specular_amount = 0.4f;
    
    if (specular_amount > 0.0f)
    {
        float spec_dot = -dot(ref, light_to_intersect);
        if (spec_dot > 0.0f)
        {
            float specular_intensity = spec_dot * pow(spec_dot, specular_coeff) * specular_amount / (light_dist * light_dist);
            light_illum += float3(specular_intensity, specular_intensity, specular_intensity);
        }
    }
    color += light_illum * float3(1, 1, 1) * 100; //color
    
    return color;
}

float3 get_ray_color(const float3& ray_pos, const float3& ray_dir)
{
    float3 ambient_lighting = float3(0.005, 0.005, 0.005);
    float3 color = float3(0.0, 0.0, 0.0);

    float2 hit = ray_march(ray_pos, ray_dir);
    float distance = hit.x;
    float material = hit.y;

    if (material > 0)
    {
        float reflect_fraction = get_reflect_fraction(material);
        float3 intersect_point = ray_pos + ray_dir * distance;
        float3 nor = estimate_normal(intersect_point);
        float3 ref = reflect(ray_dir, nor);


        if (reflect_fraction > 0)
        {
            float3 nor_r = nor;
            float3 ref_r = ref;
            float3 intersect_point_r = intersect_point;
            float3 reflected_color = float3(0, 0, 0);
            float3 k = float3(1, 1, 1);

            for (int i = 0; i < 5; i++)
            {
                hit = ray_march(intersect_point_r, ref_r);
                float distance_r = hit.x;
                float material_r = hit.y;
                if (material_r < 0) break;
                intersect_point_r = intersect_point_r + ref_r * distance_r;
                nor_r = estimate_normal(intersect_point_r);
                ref_r = reflect(ref_r, nor_r);
                float reflect_fraction_r = get_reflect_fraction(material_r);

                float3 nonreflected_color_r = get_diffuse_color(material_r) * ambient_lighting;
                nonreflected_color_r += light_in_pos(intersect_point_r, nor_r, ref_r, material_r);
                reflected_color += k * nonreflected_color_r;

                if (reflect_fraction_r <= 0.0001) break;
                k *= reflect_fraction_r;
            }
            color += reflected_color * reflect_fraction;
        }

        float3 nonreflected_color = get_diffuse_color(material) * ambient_lighting;
        if (reflect_fraction < 1) {
            nonreflected_color += light_in_pos(intersect_point, nor, ref, material);
            color += nonreflected_color * (1.0 - reflect_fraction);
        }
    }
    else {
        color = float3(0, 0, 0);
    }

    return color;
}
