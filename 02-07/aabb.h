#pragma once

#include "ray.h"
#include "rtweekend.h"
#include "vec3.h"

class Aabb {
public:
  Aabb() {}
  Aabb(const Point3 &a, const Point3 &b) {
    bounds[0] = a;
    bounds[1] = b;
  }

  Point3 min() const { return bounds[0]; }
  Point3 max() const { return bounds[1]; }

  bool hit(const Ray &r, double tmin, double tmax) const {
    // taken from "An Efficient and Robust Ray-Box Intersection Algorithm"
    float txmin, txmax, tymin, tymax, tzmin, tzmax;
    txmin = (bounds[r.getSign(0)][0] - r.origin()[0]) * r.invDirection()[0];
    txmax = (bounds[1 - r.getSign(0)][0] - r.origin()[0]) * r.invDirection()[0];
    if ((tmin > txmax) || (txmin > tmax))
      return false;
    if (txmin > tmin)
      tmin = txmin;
    if (txmax < tmax)
      tmax = txmax;
    tymin = (bounds[r.getSign(1)][1] - r.origin()[1]) * r.invDirection()[1];
    tymax = (bounds[1 - r.getSign(1)][1] - r.origin()[1]) * r.invDirection()[1];
    if ((tmin > tymax) || (tymin > tmax))
      return false;
    if (tymin > tmin)
      tmin = tymin;
    if (tymax < tmax)
      tmax = tymax;
    tzmin = (bounds[r.getSign(2)][2] - r.origin()[2]) * r.invDirection()[2];
    tzmax = (bounds[1 - r.getSign(2)][2] - r.origin()[2]) * r.invDirection()[2];
    if ((tmin > tzmax) || (tzmin > tmax))
      return false;
    if (tzmin > tmin)
      tmin = tzmin;
    if (tzmax < tmax)
      tmax = tzmax;
    return true;
  }

private:
  Point3 bounds[2];
};

inline Aabb surrounding_box(Aabb &box0, Aabb &box1) {
  Point3 small(std::min(box0.min().x(), box1.min().x()),
               fmin(box0.min().y(), box1.min().y()),
               fmin(box0.min().z(), box1.min().z()));

  Point3 big(std::max(box0.max().x(), box1.max().x()),
             fmax(box0.max().y(), box1.max().y()),
             fmax(box0.max().z(), box1.max().z()));

  return Aabb(small, big);
}
