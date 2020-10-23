#include "includes/Path-tracing.hpp"

using namespace std;

inline const Vec3f cosine_weighted_sample_on_hemisphere(float u1, float u2) {

    const float cos_theta = sqrt(1.0 - u1);
    const float sin_theta = sqrt(u1);
    const float phi = 2.0 * G_PI * u2;
    return { cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta };
}

Vec3f path_trace(Vec3f& rayorig, Vec3f& raydir, const vector<Object*>& objects) {
    Vec3f surfaceColor = 0;
    Vec3f F(1.0);
    int depth = 0;

    while (true) {
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
        if (!obj) return surfaceColor;

        // point of intersection
        Vec3f phit = rayorig + raydir * tnear;
        // normal at the intersection point
        Vec3f nhit = obj->normal(phit);
        nhit.normalize();

        surfaceColor += F * obj->emissionColor;
        F *= obj->surfaceColor;

        if (MAX_RAY_DEPTH < depth) {
            const float continue_probability = obj->surfaceColor.avg();//obj->reflection;//min(obj->surfaceColor);
            if (genRandomFloat() >= continue_probability) {
                return surfaceColor;
            }
            F = F / continue_probability;
        }

        if (obj->reflection > 0) {
            Vec3f refldir = raydir - nhit * 2 * raydir.dot(nhit);
            refldir.normalize();
            raydir = refldir;
            rayorig = phit;
        } else {
            const Vec3f w = (0.0 > raydir.dot(nhit)) ? nhit : -nhit;
            Vec3f u = (std::abs(w.x) > 0.1 ? Vec3f(0.0, 1.0, 0.0) : Vec3f(1.0, 0.0, 0.0)).cross(w);
            u.normalize();
            const Vec3f v = w.cross(u);

            const Vec3f sample_d = cosine_weighted_sample_on_hemisphere(genRandomFloat(), genRandomFloat());
            Vec3f refldir = u * sample_d.x + v * sample_d.y +  w * sample_d.z;
            refldir.normalize();
            raydir = refldir;
            rayorig = phit;
        }

        depth++;
    }

	return 0;
}