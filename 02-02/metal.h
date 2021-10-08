#include "material.h"
#include "hittable.h"
#include "vec3.h"

class Metal : public Material {
    public:
        Metal(const Color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attentuation, Ray &scattered) const override {
            Vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            do {
                scattered = Ray(rec.p, reflected + fuzz*random_in_unit_sphere(), r_in.time());
            } while (dot(scattered.direction(), rec.normal) <= 0);
            attentuation = albedo;
            return true;
        }

    private:
        Color albedo;
        double fuzz;
};
