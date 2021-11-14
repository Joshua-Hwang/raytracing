#pragma once

#include "aabb.h"
#include "material.h"
#include "ray.h"

struct HitRecord {
  Point3 p;
  Vec3 normal;
  std::shared_ptr<Material> mat_ptr;
  double t;
  double u;
  double v;
  bool front_face;

  /**
   * Coloring requires knowledge of the inner and outer sides of an object
   * If we stored the normal that is always pointing out of the surface we would
   * need to store information telling us to flip the Vec3 later during geometry
   * calculations.
   * If we stored the normal that is always pointing against the ray we would
   * need to store information telling us if the array hit the front face or
   * not. We have chosen the latter.
   */
  inline void set_face_normal(const Ray &r, const Vec3 &outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class Hittable {
public:
  // returns information in the HitRecord
  virtual bool hit(const Ray &r, double t_min, double t_max,
                   HitRecord &rec) const = 0;
  virtual bool bounding_box(double time0, double time1,
                            Aabb &output_box) const = 0;

  virtual ~Hittable() = default;
};
