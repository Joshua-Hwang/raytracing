#include "rtweekend.h"
#include "color.h"
#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "../external/lodepng/lodepng.h"

#include <algorithm>
#include <execution>
#include <iostream>
#include <numeric>
#include <chrono>
#include <pstl/glue_execution_defs.h>

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
  const unsigned image_width = 1080;
  const unsigned image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 100;

  // World
  HittableList world;
  world.add(make_shared<Sphere>(Point3(0,0,-1), 0.5));
  world.add(make_shared<Sphere>(Point3(0,-100.5,-1), 100));

  // Camera
  Camera cam;

  // Render
  Image img(image_width, image_height);
  std::vector<int> ys(image_height);
  std::iota(ys.begin(), ys.end(), 0);
  auto t1 = std::chrono::high_resolution_clock::now();
  std::for_each(std::execution::par_unseq, ys.begin(), ys.end(), [&](int y) {
    for (int x = 0; x < image_width; ++x) {
      Color pixel_color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; s++) {
        auto u = (double(x) + random_double()) / (image_width - 1);
        auto v = (double(y) + random_double()) / (image_height - 1);
        Ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, world);
      }
      img.set({x,image_height - y}, pixel_color / samples_per_pixel);
    }
  });
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> ms_double = t2 - t1;
  std::cerr << "Time taken: " << ms_double.count() << " ms" << std::endl;;
  std::cerr << "Saving file..." << std::endl;
  lodepng::encode(FILE_NAME, img.data(), image_width, image_height, LCT_RGB);
  std::cerr << "Done.\n";
  return EXIT_SUCCESS;
}
