#pragma once

#include "hittable.h"
#include "rtweekend.h"

class xyRect : public Hittable {
public:
  xyRect() {}

  xyRect(double x0, double x1, double y0, double y1, double k,
         std::shared_ptr<Material> mat)
      : mp(mat), x0(x0), x1(x1), y0(y0), y1(y1), k(k){};

  virtual bool hit(const Ray &r, double t_min, double t_max,
                   HitRecord &rec) const override;

  virtual bool bounding_box(double time0, double time1,
                            Aabb &output_box) const override {
    // The bouding box must have a non-zero width in each dimension, so pad the
    // z dimension a small amount
    output_box = Aabb(Point3(x0, y0, k - 0.0001), Point3(x1, y1, k + 0.0001));
    return true;
  }

private:
  std::shared_ptr<Material> mp;
  double x0, x1, y0, y1, k;
};

inline bool xyRect::hit(const Ray &r, double t_min, double t_max,
                        HitRecord &rec) const {
  auto t = (k - r.origin().z()) / r.direction().z();
  if (t < t_min || t > t_max) {
    return false;
  }

  auto x = r.origin().x() + t * r.direction().x();
  auto y = r.origin().y() + t * r.direction().y();

  if (x < x0 || x > x1 || y < y0 || y > y1) {
    return false;
  }

  rec.u = (x - x0) / (x1 - x0);
  rec.v = (y - y0) / (y1 - y0);

  rec.t = t;
  auto outward_normal = Vec3(0, 0, 1);
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mp;
  rec.p = r.at(t);
  return true;
}

class xzRect : public Hittable {
public:
  xzRect() {}

  xzRect(double x0, double x1, double z0, double z1, double k,
         std::shared_ptr<Material> mat)
      : mp(mat), x0(x0), x1(x1), z0(z0), z1(z1), k(k){};

  virtual bool hit(const Ray &r, double t_min, double t_max,
                   HitRecord &rec) const override;

  virtual bool bounding_box(double time0, double time1,
                            Aabb &output_box) const override {
    // The bouding box must have a non-zero width in each dimension, so pad the
    // z dimension a small amount
    output_box = Aabb(Point3(x0, k - 0.0001, z0), Point3(x1, k + 0.0001, z1));
    return true;
  }

private:
  std::shared_ptr<Material> mp;
  double x0, x1, z0, z1, k;
};

inline bool xzRect::hit(const Ray &r, double t_min, double t_max,
                        HitRecord &rec) const {
  auto t = (k - r.origin().y()) / r.direction().y();
  if (t < t_min || t > t_max) {
    return false;
  }

  auto x = r.origin().x() + t * r.direction().x();
  auto z = r.origin().z() + t * r.direction().z();

  if (x < x0 || x > x1 || z < z0 || z > z1) {
    return false;
  }

  rec.u = (x - x0) / (x1 - x0);
  rec.v = (z - z0) / (z1 - z0);

  rec.t = t;
  auto outward_normal = Vec3(0, 1, 0);
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mp;
  rec.p = r.at(t);
  return true;
}

class yzRect : public Hittable {
public:
  yzRect() {}

  yzRect(double y0, double y1, double z0, double z1, double k,
         std::shared_ptr<Material> mat)
      : mp(mat), y0(y0), y1(y1), z0(z0), z1(z1), k(k){};

  virtual bool hit(const Ray &r, double t_min, double t_max,
                   HitRecord &rec) const override;

  virtual bool bounding_box(double time0, double time1,
                            Aabb &output_box) const override {
    // The bouding box must have a non-zero width in each dimension, so pad the
    // z dimension a small amount
    output_box = Aabb(Point3(k - 0.0001, y0, z0), Point3(k + 0.0001, y1, z1));
    return true;
  }

private:
  std::shared_ptr<Material> mp;
  double y0, y1, z0, z1, k;
};

inline bool yzRect::hit(const Ray &r, double t_min, double t_max,
                        HitRecord &rec) const {
  auto t = (k - r.origin().x()) / r.direction().x();
  if (t < t_min || t > t_max) {
    return false;
  }

  auto y = r.origin().y() + t * r.direction().y();
  auto z = r.origin().z() + t * r.direction().z();

  if (y < y0 || y > y1 || z < z0 || z > z1) {
    return false;
  }

  rec.u = (y - y0) / (y1 - y0);
  rec.v = (z - z0) / (z1 - z0);

  rec.t = t;
  auto outward_normal = Vec3(1, 0, 0);
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mp;
  rec.p = r.at(t);
  return true;
}
