#pragma once

#include "material.h"
#include "hittable.h"
#include "vec3.h"

class Sphere : public Hittable {
  public:
    Sphere() {}
    Sphere(Point3 cen0, Point3 cen1, double time0, double time1, double r, std::shared_ptr<Material> m)
      : center0(cen0), center1(cen1), time0(time0), time1(time1), radius(r), mat_ptr(m) {};
    Sphere(Point3 cen, double r, std::shared_ptr<Material> m) : Sphere(cen, cen, 0.0, 1.0, r, m) {}

    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, Aabb &output_box) const override;

    Point3 center(double time) const {
      return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }

  private:
    Point3 center0, center1;
    double time0, time1;
    double radius;
    std::shared_ptr<Material> mat_ptr;

    /**
     * params:
     * p: is a point on the sphere of radius 1 centered at the origin
     * u: returned value [0,1] of angle around the Y axis from X=-1.
     * v: returned value [0,1] of angle from Y=-1 to Y=+1.
     * <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
     * <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
     * <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
     */
    static void get_sphere_uv(const Point3 &p, double &u, double &v) {
        auto theta = std::acos(-p.y());
        auto phi = std::atan2(-p.z(), p.x()) + pi;

        u = phi / (2*pi);
        v = theta / pi;
    }
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
  get_sphere_uv(outward_normal, rec.u, rec.v);
  rec.mat_ptr = mat_ptr;
  return true;
}

inline bool Sphere::bounding_box(double time0, double time1, Aabb &output_box) const {
  Aabb box0(
    center(time0) - Vec3(radius, radius, radius),
    center(time0) + Vec3(radius, radius, radius));
    
  Aabb box1(
    center(time1) - Vec3(radius, radius, radius),
    center(time1) + Vec3(radius, radius, radius));

  output_box = surrounding_box(box0, box1);
  return true;
}
