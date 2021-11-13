#pragma once
#include "vec3.h"
#include "ray.h"
#include "rtweekend.h"

struct HitRecord;

class Material {
    public:
        virtual Color emitted(double u, double v, const Point3 &p) const {
            return Color(0, 0, 0);
        }
        
        virtual bool scatter(
            const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const = 0;
};