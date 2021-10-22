#include "material.h"
#include "hittable.h"
#include "vec3.h"
#include "texture.h"

class Lambertian : public Material {
    public:
        Lambertian(const Color &a) : albedo(std::make_shared<SolidColor>(a)) {}
        Lambertian(const std::shared_ptr<Texture> a) : albedo(a) {}

        virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attentuation, Ray &scattered) const override {
            // lambertian diffuse
            // Point3 target = rec.p + rec.normal + random_unit_vector();
            // hemispherical scattering
            Vec3 scatter_direction = rec.normal + random_in_hemisphere(rec.normal);
            if (scatter_direction.near_zero()) {
                scatter_direction = rec.normal;
            }
            scattered = Ray(rec.p, scatter_direction, r_in.time());
            attentuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    private:
        std::shared_ptr<Texture> albedo;
};
