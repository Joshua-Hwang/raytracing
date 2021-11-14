#pragma once

#include "aabb.h"
#include "hittable.h"
#include "rtweekend.h"

#include "aarect.h"
#include "hittable_list.h"
#include "vec3.h"
#include <memory>

class Box : public Hittable {
public:
  Box() {}
  Box(const Point3 &p0, const Point3 &p1, std::shared_ptr<Material> ptr);
  ~Box() = default;

  virtual bool hit(const Ray &r, double t_min, double t_max,
                   HitRecord &rec) const override;

  virtual bool bounding_box(double time0, double time1,
                            Aabb &output_box) const override {
    output_box = Aabb(box_min, box_max);
    return true;
  }

private:
  Point3 box_min;
  Point3 box_max;
  HittableList sides;
};

Box::Box(const Point3 &p0, const Point3 &p1, std::shared_ptr<Material> ptr) {
  box_min = p0;
  box_max = p1;

  sides.add(
      std::make_shared<xyRect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
  sides.add(
      std::make_shared<xyRect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

  sides.add(
      std::make_shared<xzRect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
  sides.add(
      std::make_shared<xzRect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

  sides.add(
      std::make_shared<yzRect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
  sides.add(
      std::make_shared<yzRect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
}

inline bool Box::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const {
  return sides.hit(r, t_min, t_max, rec);
}
