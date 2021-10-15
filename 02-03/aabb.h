#pragma once

#include "ray.h"
#include "vec3.h"
#include "rtweekend.h"

class Aabb {
  public:
    Aabb() {}
    Aabb(const Point3 &a, const Point3& b) { minimum = a; maximum = b; }

    Point3 min() const { return minimum; }
    Point3 max() const { return maximum; }

    bool hit(const Ray &r, double t0, double t1) const {
      // taken from "An Efficient and Robust Ray-Box Intersection Algorithm"
      float tmin, tmax, tymin, tymax, tzmin, tzmax;
      tmin = (minimum[r.getSign(0)] - r.origin()[0]) * r.invDirection()[0];
      tmax = (minimum[1-r.getSign(0)] - r.origin()[0]) * r.invDirection()[0];
      tymin = (minimum[r.getSign(1)] - r.origin()[1]) * r.invDirection()[1];
      tymax = (minimum[1-r.getSign(1)] - r.origin()[1]) * r.invDirection()[1];
      if ( (tmin > tymax) || (tymin > tmax) ) return false;
      if (tymin > tmin) tmin = tymin;
      if (tymax < tmax) tmax = tymax;
      tzmin = (minimum[r.getSign(2)] - r.origin()[2]) * r.invDirection()[2];
      tzmax = (minimum[1-r.getSign(2)] - r.origin()[2]) * r.invDirection()[2];
      if ( (tmin > tzmax) || (tzmin > tmax) ) return false;
      if (tzmin > tmin) tmin = tzmin;
      if (tzmax < tmax) tmax = tzmax;
      return ( (tmin < t1) && (tmax > t0) );
    }

  private:
    Point3 minimum;
    Point3 maximum;
};

inline Aabb surrounding_box(Aabb &box0, Aabb &box1) {
  Point3 small(std::min(box0.min().x(), box1.min().x()),
    fmin(box0.min().y(), box1.min().y()),
    fmin(box0.min().z(), box1.min().z()));

  Point3 big(std::max(box0.max().x(), box1.max().x()),
    fmax(box0.max().y(), box1.max().y()),
    fmax(box0.max().z(), box1.max().z()));

  return Aabb(small,big);
}
