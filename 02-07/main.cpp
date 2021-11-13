#include "rtweekend.h"
#include "color.h"
#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"
#include "lambertian.h"
#include "metal.h"
#include "dielectric.h"
#include "light.h"
#include "bvh.h"
#include "checker_texture.h"
#include "noise_texture.h"
#include "image_texture.h"

#include "../external/lodepng/lodepng.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <execution>
#include <iostream>
#include <numeric>
#include <chrono>
#include <mutex>

#define FILE_NAME "out.png"

Color ray_color(const Ray &r, const Color &background, const Hittable &world, int depth) {
  HitRecord rec;

  // Stop recursion
  if (depth <= 0) {
      return Color(0, 0, 0);
  }
  if (!world.hit(r, 0.001, inf, rec)) {
      return background;
  }
  Ray scattered;
  Color attenuation;
  Color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

  if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return emitted;
  }
  return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

void write_color(Image &img, int x, int y, Color pixel_color) {
  auto r = std::sqrt(pixel_color.x());
  auto g = std::sqrt(pixel_color.y());
  auto b = std::sqrt(pixel_color.z());

  // gamma-correct for gamma=2.0
  img.set({x, img.height - y - 1}, Color(r, g, b));
}

HittableList random_scene() {
  HittableList world;

  auto checker = std::make_shared<CheckerTexture>(Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
  world.add(std::make_shared<Sphere>(Point3(0, -1000, 0), Point3(0, -1000, 0), 0.0, 1.0, 1000, std::make_shared<Lambertian>(checker)));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      Point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
      Point3 center2 = Vec3(center);

      if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
        std::shared_ptr<Material> sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = Color::random() * Color::random();
          sphere_material = std::make_shared<Lambertian>(albedo);
          center2 = center + Vec3(0, random_double(0, 0.5), 0);
        } else if (choose_mat < 0.95) {
          // metal
          auto albedo = Color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = std::make_shared<Metal>(albedo, fuzz);
        } else {
          // glass
          sphere_material = std::make_shared<Dielectric>(1.5);
        }

        world.add(std::make_shared<Sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
      }
    }
  }

  auto material1 = std::make_shared<Dielectric>(1.5);
  world.add(std::make_shared<Sphere>(Point3(0, 1, 0), Point3(0, 1, 0), 0.0, 1.0, 1.0, material1));

  auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
  world.add(std::make_shared<Sphere>(Point3(-4, 1, 0), Point3(-4, 1, 0), 0.0, 1.0, 1.0, material2));

  auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
  world.add(std::make_shared<Sphere>(Point3(4, 1, 0), Point3(4, 1, 0), 0.0, 1.0, 1.0, material3));

  return world;
}

HittableList two_perlin_spheres() {
    HittableList objects;

    auto perlin_texture = std::make_shared<NoiseTexture>(10);
    objects.add(std::make_shared<Sphere>(Point3(0, -1000, 0), Point3(0, -1000, 0), 0.0, 1.0, 1000, std::make_shared<Lambertian>(perlin_texture)));
    objects.add(std::make_shared<Sphere>(Point3(0, 2, 0), Point3(0, 2, 0), 0.0, 1.0, 2, std::make_shared<Lambertian>(perlin_texture)));

    return objects;
}

HittableList earth() {
    auto earth_texture = std::make_shared<ImageTexture>("earthmap.png");
    auto earth_surface = std::make_shared<Lambertian>(earth_texture);
    auto globe = std::make_shared<Sphere>(Point3(0, 0, 0), 2, earth_surface);

    return HittableList(globe);
}

int main(int argc, char *argv[]) {
  // Image
  const auto aspect_ratio = 3.0 / 2.0;
  const unsigned image_width = 300;
  const unsigned image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 500;
  const int max_depth = 50;
  const double time0 = 0.0;
  const double time1 = 0.5;

  // World
  Color background(0, 0, 0);
  //auto world = BvhNode(random_scene(), time0, time1);
  //auto world = two_perlin_spheres();
  auto world = earth();

  // Camera
  Point3 lookfrom(13, 2, 3);
  Point3 lookat(0, 0, 0);
  Vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;
  Camera cam(lookfrom, lookat, vup, 20.0, aspect_ratio, aperture, dist_to_focus, time0, time1);

  // Render
  Image img(image_width, image_height);
  std::vector<int> ys(image_height);
  std::iota(ys.begin(), ys.end(), 0);
  int lines(0);
  std::mutex lines_mutex;
  auto t1 = std::chrono::high_resolution_clock::now();
  std::for_each(std::execution::par_unseq, ys.begin(), ys.end(), [&](int y) {
    for (int x = 0; x < image_width; ++x) {
      Color pixel_color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; s++) {
        auto u = (x + random_double()) / (image_width - 1);
        auto v = (y + random_double()) / (image_height - 1);
        Ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, background, world, max_depth);
      }
      write_color(img, x, y, pixel_color / samples_per_pixel);
    }
    lines_mutex.lock();
    lines++;
    std::cout << "\rFinished line " << lines << " of " << image_height << " lines" << std::flush;
    lines_mutex.unlock();
  });
  std::cout << std::endl;

  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> ms_double = t2 - t1;
  std::cerr << "Time taken: " << ms_double.count() << " ms" << std::endl;;
  std::cerr << "Saving file..." << std::endl;
  lodepng::encode(FILE_NAME, img.data(), image_width, image_height, LCT_RGB);
  std::cerr << "Done.\n";
  return EXIT_SUCCESS;
}
