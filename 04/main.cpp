#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "../external/lodepng/lodepng.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#define FILE_NAME "out.png"

/**
* If hitpoint exists
* will return the distance to the sphere
* else
* will return -1
*/
double hit_sphere(const Point3 &center, double radius, const Ray &r) {
  // from origin to center
  Vec3 oc = r.origin() - center;
  auto a = r.direction().length_squared();
  auto half_b = dot(oc, r.direction());
  auto c = oc.length_squared() - radius * radius;
  auto discriminant = half_b * half_b - a * c;
  // the discriminant is all that is needed. If a solution exists
  // (discriminant == 0) then we have a single solution, the ray grazes the sphere
  // (discriminant > 0) then we two solution and the ray entered the sphere
  if (discriminant < 0) {
    return -1.0;
  }
  // some simplification of the quadratic formula
  return (-half_b - sqrt(discriminant)) / a;
}

Color ray_color(const Ray &r) {
  auto t = hit_sphere(Point3(0, 0, -1), 0.5, r);
  // sphere location placed at z = -1 right in front of the camera
  if (t > 0.0) {
    // normal vector
    Vec3 N = unit_vector(r.at(t) - Vec3(0, 0, -1));
    // to avoid negative colors
    return 0.5 * Color(N.x() + 1, N.y() + 1, N.z() + 1);
  }
  // Didn't hit anything so this is empty air
  Vec3 unit_direction = unit_vector(r.direction());
  // t is arbitrarily set as a single step of the y direction
  t = 0.5 * (unit_direction.y() + 1.0);
  // As time continues the ray loses color.
  return (t) * Color(1.0, 1.0, 1.0) + (1.0 - t) * Color(0.5, 0.7, 1.0);
}

int main(int argc, char *argv[]) {
  // Image
  const auto aspect_ratio = 16.0 / 9.0;
  const unsigned image_width = 400;
  const unsigned image_height = static_cast<int>(image_width / aspect_ratio);

  // Camera
  auto viewport_height = 2.0;
  auto viewport_width = aspect_ratio * viewport_height;
  auto focal_length = 1.0;

  auto origin = Point3(0, 0, 0); // centre of screen
  auto horizontal = Vec3(viewport_width, 0, 0); // width defined as vector
  auto vertical = Vec3(0, viewport_height, 0);
  // lower_left_corner will be used to build rays. It makes sense this needs a
  // z component if we want to rays to go anywhere
  // as the name suggests it points to the most bottom left ray. The for loop builds
  // the other rays from this point for implementation simplicity
  auto upper_left_corner = origin - horizontal/2 - vertical/2 - Vec3(0, 0, focal_length);

  // Render
  Image img(image_width, image_height);
  for (int y = 0; y < image_height; ++y) {
    std::cerr << "\rScanlines remaining: " << y << ' ' << std::flush;

    for (int x = 0; x < image_width; ++x) {
      auto u = double(x) / (image_width - 1);
      auto v = double(y) / (image_height - 1);
      Ray r(origin, upper_left_corner + u * horizontal + v * vertical - origin);
      Color pixel_color = ray_color(r);
      img.set({x,y}, pixel_color);
    }
  }
  std::cerr << "\nSaving file...\n";
  lodepng::encode(FILE_NAME, img.data(), image_width, image_height, LCT_RGB);
  std::cerr << "Done.\n";
  return EXIT_SUCCESS;
}
