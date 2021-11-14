#pragma once

#include "vec3.h"

class Ray {
public:
  Ray() {}
  Ray(const Point3 &origin, const Vec3 &direction, double time = 0.0)
      : orig(origin), dir(direction), tm(time) {
    invDir = Vec3(1 / direction.x(), 1 / direction.y(), 1 / direction.z());
    sign[0] = (invDir.x() < 0);
    sign[1] = (invDir.y() < 0);
    sign[2] = (invDir.z() < 0);
  }

  Point3 origin() const { return orig; }
  Vec3 direction() const { return dir; }
  Vec3 invDirection() const { return invDir; }
  int getSign(int axis) const { return sign[axis]; }
  double time() const { return tm; }

  Point3 at(double t) const { return orig + t * dir; }

private:
  Point3 orig;
  Vec3 dir;
  Vec3 invDir;
  int sign[3];
  double tm;
};
