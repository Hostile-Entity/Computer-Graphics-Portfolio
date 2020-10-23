#include "includes/Ray-tracing.hpp"

Vec3f getSurfaceColor(const std::vector<Object*>& spheres, const Object* sphere, Vec3f & phit, Vec3f nhit) {
    Vec3f surfaceColor = 0;

    // it's a diffuse object, no need to raytrace any further
    for (unsigned i = 0; i < spheres.size(); ++i) {
        if (spheres[i]->emissionColor.x > 0) {
            // this is a light
            Vec3f transmission = 1;
            Vec3f lightDirection = spheres[i]->center - phit;
            lightDirection.normalize();
            float distanceToLight = phit.distance(spheres[i]->center);
            for (unsigned j = 0; j < spheres.size(); ++j) {
                if (i != j) {
                    float t0, t1;
                    if (spheres[j]->intersect(phit + nhit * EPS, lightDirection, t0, t1)) {
                        if (t0 > 0 && t0 < distanceToLight) {
                            transmission = 0.1;
                        }
                    }
                }
            }
            surfaceColor += sphere->surfaceColor * transmission *
                std::max(float(0), nhit.dot(lightDirection)) * spheres[i]->emissionColor;
        }
    }
    return surfaceColor;
}

Vec3f ray_trace(
    const Vec3f& rayorig,
    const Vec3f& raydir,
    const std::vector<Object*>& objects,
    const int& depth)
{
    float tnear = INFINITY;
    const Object* obj = NULL;
    // find intersection of this ray with the sphere in the scene
    for (unsigned i = 0; i < objects.size(); ++i) {
        float t0 = INFINITY, t1 = INFINITY;
        if (objects[i]->intersect(rayorig, raydir, t0, t1)) {

            if (t0 < 0) t0 = t1;
            if (t0 < tnear) {
                tnear = t0;
                obj = objects[i];
            }
        }
    }
    // if there's no intersection return black or background color
    if (!obj) return Vec3f(0);
    // color of the ray/surfaceof the object intersected by the ray
    Vec3f surfaceColor = 0;
    // point of intersection
    Vec3f phit = rayorig + raydir * tnear;
    // normal at the intersection point
    Vec3f nhit = obj->normal(phit);
    // normalize normal direction 
    nhit.normalize();

    if (raydir.dot(nhit) > 0) nhit = -nhit;
    if (obj->reflection > 0 && depth < MAX_RAY_DEPTH) {
        float facingratio = -raydir.dot(nhit);
        // compute reflection direction (not need to normalize because all vectors
        // are already normalized)
        Vec3f refldir = raydir - nhit * 2 * raydir.dot(nhit);
        refldir.normalize();
        Vec3f reflection = ray_trace(phit + nhit * EPS, refldir, objects, depth + 1);
        // the result is a mix of reflection and sphere color
        surfaceColor += reflection * obj->reflection;
    }
    surfaceColor += getSurfaceColor(objects, obj, phit, nhit) * (1 - obj->reflection);

    return surfaceColor + obj->emissionColor;
}
