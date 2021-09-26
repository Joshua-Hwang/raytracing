#include "material.h"
#include "hittable.h"
#include "vec3.h"

class Light : public Material {
    public:
        Light(const Color &a) : albedo(a) {}

        virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attentuation, Ray &scattered) const override {
            attentuation = albedo;
            return false;
        }

    private:
        Color albedo;
};