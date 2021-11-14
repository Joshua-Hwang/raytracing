#include "material.h"
#include "texture.h"

class DiffuseLight : public Material {
public:
  DiffuseLight(std::shared_ptr<Texture> a) : emit(a) {}
  DiffuseLight(Color c) : emit(std::make_shared<SolidColor>(c)) {}

  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       Color &attentuation, Ray &scattered) const override {
    return false;
  }

  virtual Color emitted(double u, double v, const Point3 &p) const override {
    return emit->value(u, v, p);
  }

private:
  std::shared_ptr<Texture> emit;
};