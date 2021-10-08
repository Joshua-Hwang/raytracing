#include "material.h"
#include "hittable.h"
#include "vec3.h"

class Dielectric : public Material {
    public:
        Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attentuation, Ray &scattered) const override {
            attentuation = Color(1.0, 1.0, 1.0);
            double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

            Vec3 unit_direction = unit_vector(r_in.direction());
            double cos_theta = std::min(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

            Vec3 direction = refraction_ratio * sin_theta > 1.0 || reflectance(cos_theta, refraction_ratio) > random_double()
                ? reflect(unit_direction, rec.normal)
                : refract(unit_direction, rec.normal, refraction_ratio);
                
            scattered = Ray(rec.p, direction);
            return true;
        }

    private:
        double ir;

        static double reflectance(double cosine, double ref_idx) {
            // Use Schlick's approximation for reflectance
            auto r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0;
            return r0 + (1 - r0)*pow((1 - cosine), 5);
        }
};