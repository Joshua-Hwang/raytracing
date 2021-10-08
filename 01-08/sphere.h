#pragma once

#include "material.h"
#include "hittable.h"
#include "vec3.h"

class Sphere : public Hittable {
  public:
    Sphere() {}
    Sphere(Point3 cen, double r, std::shared_ptr<Material> m) : center(cen), radius(r), mat_ptr(m) {};

    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;

  private:
    Point3 center;
    double radius;
    std::shared_ptr<Material> mat_ptr;
};

inline bool Sphere::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const {
  // from origin to center
  Vec3 oc = r.origin() - center;
  auto a = r.direction().length_squared();
  auto half_b = dot(oc, r.direction());
  auto c = oc.length_squared() - radius * radius;
  auto discriminant = half_b * half_b - a * c;
  // the discriminant is all that is needed. If a solution exists
  // (discriminant == 0) then we have a single solution, the ray grazes the sphere
  // (discriminant > 0) then we two solution and the ray entered the sphere
  if (discriminant < 0) {
    return false;
  }
  auto sqrtd = sqrt(discriminant);
  // find nearest root that lies in the acceptable range
  // some simplification of the quadratic formula
  auto root = (-half_b - sqrtd) / a;
  if (root < t_min || t_max < root) {
    // try other root
    root = (-half_b + sqrtd) / a;
    if (root < t_min || t_max < root) {
      return false;
    }
  }

  rec.t = root;
  rec.p = r.at(rec.t);
  Vec3 outward_normal = (rec.p - center) / radius; // unit Vec3
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mat_ptr;
  return true;
}
