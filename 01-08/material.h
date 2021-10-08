#pragma once
#include "vec3.h"
#include "ray.h"
#include "rtweekend.h"

struct HitRecord;

class Material {
    public:
        virtual bool scatter(
            const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const = 0;
};