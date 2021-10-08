#pragma once

#include "material.h"
#include "hittable.h"
#include "vec3.h"

class Sphere : public Hittable {
  public:
    Sphere() {}
    Sphere(Point3 cen0, Point3 cen1, double time0, double time1, double r, std::shared_ptr<Material> m)
      : center0(cen0), center1(cen1), time0(time0), time1(time1), radius(r), mat_ptr(m) {};

    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;

    Point3 center(double time) const {
      return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }

  private:
    Point3 center0, center1;
    double time0, time1;
    double radius;
    std::shared_ptr<Material> mat_ptr;
};

inline bool Sphere::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const {
  // from origin to center
  Vec3 oc = r.origin() - center(r.time());
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
  Vec3 outward_normal = (rec.p - center(r.time())) / radius; // unit Vec3
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mat_ptr;
  return true;
}
