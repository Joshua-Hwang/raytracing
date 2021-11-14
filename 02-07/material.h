#pragma once
#include "ray.h"
#include "rtweekend.h"
#include "vec3.h"

struct HitRecord;

class Material {
public:
  ~Material() = default;

  virtual Color emitted(double u, double v, const Point3 &p) const {
    return Color(0, 0, 0);
  }

  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       Color &attenuation, Ray &scattered) const = 0;
};
