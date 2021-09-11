#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "../external/lodepng/lodepng.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#define FILE_NAME "out.png"

bool hit_sphere(const Point3 &center, double radius, const Ray &r) {
  // from origin to center
  Vec3 oc = r.origin() - center;
  auto a = dot(r.direction(), r.direction());
  auto b = 2.0 * dot(oc, r.direction());
  auto c = dot(oc, oc) - radius*radius;
  auto discriminant = b * b - 4 * a * c;
  // the discriminant is all that is needed. If a solution exists
  // (discriminant == 0) then we have a single solution, the ray grazes the sphere
  // (discriminant > 0) then we two solution and the ray entered the sphere
  return (discriminant > 0);
}

Color ray_color(const Ray &r) {
  // sphere location placed at z = -1 right in front of the camera
  if (hit_sphere(Point3(0, 0, -1), 0.5, r)) {
    return Color(1, 0, 0);
  }
  Vec3 unit_direction = unit_vector(r.direction());
  // t is arbitrarily set as a single step of the y direction
  auto t = 0.5 * (unit_direction.y() + 1.0);
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
