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

class Translate final : public Hittable {
    public:
    Translate(std::shared_ptr<Hittable> p, const Vec3 &displacement) : ptr(p), offset(displacement) {}
    bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, Aabb &output_box) const override;

    private:
    std::shared_ptr<Hittable> ptr;
    Vec3 offset;
};

bool Translate::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const {
    Ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (!ptr->hit(moved_r, t_min, t_max, rec)) {
        return false;
    }
    rec.p += offset;
    rec.set_face_normal(moved_r, rec.normal);

    return true;
}

bool Translate::bounding_box(double time0, double time1, Aabb &output_box) const {
    if (!ptr->bounding_box(time0, time1, output_box)) {
        return false;
    }
    output_box = Aabb(output_box.min() + offset, output_box.max() + offset);

    return true;
}

class rotateY : public Hittable {
    public:
    rotateY(std::shared_ptr<Hittable> p, double angle);

    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;

    virtual bool bounding_box(double time0, double time1, Aabb &output_box) const override {
        output_box = bbox;
        return hasbox;
    }

    private:
    std::shared_ptr<Hittable> ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    Aabb bbox;
};

rotateY::rotateY(std::shared_ptr<Hittable> p, double angle) : ptr(p) {
    auto radians = degrees_to_radians(angle);
    sin_theta = std::sin(radians);
    cos_theta = std::cos(radians);
    hasbox = ptr->bounding_box(0,1,bbox);

    Point3 min(inf, inf, inf);
    Point3 max(-inf, -inf, -inf);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                auto x = i*bbox.max().x() + (1-i)*bbox.min().x();
                auto y = j*bbox.max().y() + (1-j)*bbox.min().y();
                auto z = k*bbox.max().z() + (1-k)*bbox.min().z();

                auto newx = cos_theta*x + sin_theta*z;
                auto newz = -sin_theta*x + cos_theta*z;

                Vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
                    min[c] = std::min(min[c], tester[c]);
                    max[c] = std::max(max[c], tester[c]);
                }
            }
        }
    }

    bbox = Aabb(min, max);
}

bool rotateY::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const {
    auto origin = r.origin();
    auto direction = r.direction();

    origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
    origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

    direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
    direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

    Ray rotated_r(origin, direction, r.time());

    if (!ptr->hit(rotated_r, t_min, t_max, rec)) {
        return false;
    }

    auto p = rec.p;
    auto normal = rec.normal;

    p[0] = cos_theta*rec.p[0] + sin_theta*rec.p[2];
    p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

    normal[0] = cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
    normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

    rec.p = p;
    rec.set_face_normal(rotated_r, normal);

    return true;
}