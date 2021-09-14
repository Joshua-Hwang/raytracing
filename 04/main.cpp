#include "rtweekend.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "../external/lodepng/lodepng.h"

#include <iostream>

using std::make_shared;

#define FILE_NAME "out.png"

Color ray_color(const Ray &r, const Hittable &world) {
  HitRecord rec;
  if (world.hit(r, 0, infinity, rec)) {
    return 0.5 * (rec.normal + Color(1,1,1));
  }
  // Didn't hit anything so this is empty air
  Vec3 unit_direction = unit_vector(r.direction());
  // t is arbitrarily set as a single step of the y direction
  auto t = 0.5 * (unit_direction.y() + 1.0);
  // As time continues the ray loses color.
  return (1.0 - t) * Color(1.0, 1.0, 1.0) + (t) * Color(0.5, 0.7, 1.0);
}

int main(int argc, char *argv[]) {
  // Image
  const auto aspect_ratio = 16.0 / 9.0;
  const unsigned image_width = 400;
  const unsigned image_height = static_cast<int>(image_width / aspect_ratio);

  // World
  HittableList world;
  world.add(make_shared<Sphere>(Point3(0,0,-1), 0.5));
  world.add(make_shared<Sphere>(Point3(0,-100.5,-1), 100));

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
  auto lower_left_corner = origin - horizontal/2 - vertical/2 - Vec3(0, 0, focal_length);

  // Render
  Image img(image_width, image_height);
  for (int y = 0; y < image_height; ++y) {
    std::cerr << "\rScanlines remaining: " << y << ' ' << std::flush;

    for (int x = 0; x < image_width; ++x) {
      auto u = double(x) / (image_width - 1);
      auto v = double(y) / (image_height - 1);
      Ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
      Color pixel_color = ray_color(r, world);
      img.set({x,image_height - y}, pixel_color);
    }
  }
  std::cerr << "\nSaving file...\n";
  lodepng::encode(FILE_NAME, img.data(), image_width, image_height, LCT_RGB);
  std::cerr << "Done.\n";
  return EXIT_SUCCESS;
}
