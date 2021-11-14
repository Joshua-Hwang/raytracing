#pragma once

#include "texture.h"

class CheckerTexture : public Texture {
public:
  CheckerTexture() {}
  CheckerTexture(std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd)
      : even(even), odd(odd) {}
  CheckerTexture(Color c1, Color c2)
      : even(std::make_shared<SolidColor>(c1)),
        odd(std::make_shared<SolidColor>(c2)) {}
  virtual ~CheckerTexture() = default;

  virtual Color value(double u, double v, const Point3 &p) const override {
    auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
    if (sines < 0) {
      return odd->value(u, v, p);
    } else {
      return even->value(u, v, p);
    }
  }

private:
  std::shared_ptr<Texture> even;
  std::shared_ptr<Texture> odd;
};
