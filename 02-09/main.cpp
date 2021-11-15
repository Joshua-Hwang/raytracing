#include "aarect.h"
#include "box.h"
#include "bvh.h"
#include "camera.h"
#include "checker_texture.h"
#include "color.h"
#include "dielectric.h"
#include "diffuse_light.h"
#include "hittable_list.h"
#include "image_texture.h"
#include "lambertian.h"
#include "light.h"
#include "material.h"
#include "metal.h"
#include "noise_texture.h"
#include "rtweekend.h"
#include "sphere.h"
#include "constant_medium.h"

#include "../external/lodepng/lodepng.h"
#include "vec3.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <execution>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>

#define FILE_NAME "out.png"

Color ray_color(const Ray &r, const Color &background, const Hittable &world,
                int depth) {
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
  return emitted +
         attenuation * ray_color(scattered, background, world, depth - 1);
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

  auto checker = std::make_shared<CheckerTexture>(Color(0.2, 0.3, 0.1),
                                                  Color(0.9, 0.9, 0.9));
  world.add(std::make_shared<Sphere>(Point3(0, -1000, 0), Point3(0, -1000, 0),
                                     0.0, 1.0, 1000,
                                     std::make_shared<Lambertian>(checker)));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      Point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
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

        world.add(std::make_shared<Sphere>(center, center2, 0.0, 1.0, 0.2,
                                           sphere_material));
      }
    }
  }

  auto material1 = std::make_shared<Dielectric>(1.5);
  world.add(std::make_shared<Sphere>(Point3(0, 1, 0), Point3(0, 1, 0), 0.0, 1.0,
                                     1.0, material1));

  auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
  world.add(std::make_shared<Sphere>(Point3(-4, 1, 0), Point3(-4, 1, 0), 0.0,
                                     1.0, 1.0, material2));

  auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
  world.add(std::make_shared<Sphere>(Point3(4, 1, 0), Point3(4, 1, 0), 0.0, 1.0,
                                     1.0, material3));

  return world;
}

HittableList two_perlin_spheres() {
  HittableList objects;

  auto perlin_texture = std::make_shared<NoiseTexture>(10);
  objects.add(std::make_shared<Sphere>(
      Point3(0, -1000, 0), Point3(0, -1000, 0), 0.0, 1.0, 1000,
      std::make_shared<Lambertian>(perlin_texture)));
  objects.add(
      std::make_shared<Sphere>(Point3(0, 2, 0), Point3(0, 2, 0), 0.0, 1.0, 2,
                               std::make_shared<Lambertian>(perlin_texture)));

  return objects;
}

HittableList earth() {
  auto earth_texture = std::make_shared<ImageTexture>("earthmap.png");
  auto earth_surface = std::make_shared<Lambertian>(earth_texture);
  auto globe = std::make_shared<Sphere>(Point3(0, 0, 0), 2, earth_surface);

  return HittableList(globe);
}

HittableList simple_light() {
  HittableList objects;

  auto pertext = std::make_shared<NoiseTexture>(4);
  objects.add(std::make_shared<Sphere>(Point3(0, -1000, 0), Point3(0, -1000, 0),
                                       0.0, 1.0, 1000,
                                       std::make_shared<Lambertian>(pertext)));
  objects.add(std::make_shared<Sphere>(Point3(0, 2, 0), Point3(0, 2, 0), 0.0,
                                       1.0, 2,
                                       std::make_shared<Lambertian>(pertext)));

  auto difflight = std::make_shared<DiffuseLight>(Color(4, 4, 4));
  objects.add(std::make_shared<xyRect>(3, 5, 1, 3, -2, difflight));

  return objects;
}

HittableList cornell_box() {
  HittableList objects;

  // walls
  auto red = std::make_shared<Lambertian>(Color(0.65, 0.05, 0.05));
  auto white = std::make_shared<Lambertian>(Color(0.73, 0.73, 0.73));
  auto green = std::make_shared<Lambertian>(Color(0.12, 0.45, 0.15));
  auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

  objects.add(std::make_shared<yzRect>(0, 555, 0, 555, 555, green));
  objects.add(std::make_shared<yzRect>(0, 555, 0, 555, 0, red));
  objects.add(std::make_shared<xzRect>(213, 343, 227, 332, 554, light));
  objects.add(std::make_shared<xzRect>(0, 555, 0, 555, 0, white));
  objects.add(std::make_shared<xzRect>(0, 555, 0, 555, 555, white));
  objects.add(std::make_shared<xyRect>(0, 555, 0, 555, 555, white));

  // boxes
  std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
  box1 = std::make_shared<rotateY>(box1, 15);
  box1 = std::make_shared<Translate>(box1, Vec3(265, 0, 295));  
  objects.add(box1);

  std::shared_ptr<Hittable> box2 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
  box2 = std::make_shared<rotateY>(box2, -18);
  box2 = std::make_shared<Translate>(box2, Vec3(130,0,65));
  objects.add(box2);

  return objects;
}

HittableList cornell_smoke() {
  HittableList objects;

  // walls
  auto red = std::make_shared<Lambertian>(Color(0.65, 0.05, 0.05));
  auto white = std::make_shared<Lambertian>(Color(0.73, 0.73, 0.73));
  auto green = std::make_shared<Lambertian>(Color(0.12, 0.45, 0.15));
  auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

  objects.add(std::make_shared<yzRect>(0, 555, 0, 555, 555, green));
  objects.add(std::make_shared<yzRect>(0, 555, 0, 555, 0, red));
  objects.add(std::make_shared<xzRect>(213, 343, 227, 332, 554, light));
  objects.add(std::make_shared<xzRect>(0, 555, 0, 555, 0, white));
  objects.add(std::make_shared<xzRect>(0, 555, 0, 555, 555, white));
  objects.add(std::make_shared<xyRect>(0, 555, 0, 555, 555, white));

  // boxes
  std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
  box1 = std::make_shared<rotateY>(box1, 15);
  box1 = std::make_shared<Translate>(box1, Vec3(265, 0, 295));  
  objects.add(std::make_shared<ConstantMedium>(box1, 0.01, Color(0,0,0)));

  std::shared_ptr<Hittable> box2 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
  box2 = std::make_shared<rotateY>(box2, -18);
  box2 = std::make_shared<Translate>(box2, Vec3(130,0,65));
  objects.add(std::make_shared<ConstantMedium>(box2, 0.01, Color(1,1,1)));

  return objects;
}

HittableList final_scene() {
    HittableList boxes1;
    auto ground = std::make_shared<Lambertian>(Color(0.48,0.83,0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i< boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(std::make_shared<Box>(Point3(x0, y0, z0), Point3(x1,y1,z1), ground));
        }
    }

    HittableList objects;
    objects.add(std::make_shared<BvhNode>(boxes1, 0, 1));

    auto light = std::make_shared<DiffuseLight>(Color(7,7,7));
    objects.add(std::make_shared<xzRect>(123,423,149,412,554,light));

    auto center1 = Point3(400,400,200);
    auto center2 = center1 + Vec3(30, 0, 0);
    auto moving_sphere_material = std::make_shared<Lambertian>(Color(0.7,0.3,0.1));
    objects.add(std::make_shared<Sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(std::make_shared<Sphere>(Point3(260,150,45), 50, std::make_shared<Dielectric>(1.5)));
    objects.add(std::make_shared<Sphere>(Point3(0,150,145),50,std::make_shared<Metal>(Color(0.8,0.8,0.9), 1.0)));

    auto boundary = std::make_shared<Sphere>(Point3(360,150,145), 70, std::make_shared<Dielectric>(1.5));
    objects.add(boundary);
    objects.add(std::make_shared<ConstantMedium>(boundary, 0.2, Color(0.2,0.4,0.9)));
    boundary = std::make_shared<Sphere>(Point3(0,0,0), 5000, std::make_shared<Dielectric>(1.5));
    objects.add(std::make_shared<ConstantMedium>(boundary, 0.0001, Color(1,1,1)));

    auto emat = std::make_shared<Lambertian>(std::make_shared<ImageTexture>("earthmap.png"));
    objects.add(std::make_shared<Sphere>(Point3(400,200,400), 100, emat));
    auto pertext = std::make_shared<NoiseTexture>(0.1);
    objects.add(std::make_shared<Sphere>(Point3(220,280,300), 80, std::make_shared<Lambertian>(pertext)));

    HittableList boxes2;
    auto white = std::make_shared<Lambertian>(Color(0.73,0.73,0.73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(std::make_shared<Sphere>(Point3::random(0,165), 10, white));
    }

    objects.add(std::make_shared<Translate>(std::make_shared<rotateY>(std::make_shared<BvhNode>(boxes2, 0.0, 1.0), 15), Vec3(-100, 270, 395)));

    return objects;
}

int main(int argc, char *argv[]) {
  // World
  // auto world = BvhNode(random_scene(), time0, time1);
  // auto world = two_perlin_spheres();
  // auto world = earth();
  // auto world = simple_light();
  // auto world = cornell_smoke();
  auto world = final_scene();

  // Image
  const auto aspect_ratio = 1.0;
  const unsigned image_width = 1920;
  const unsigned image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 10000;
  const int max_depth = 50;
  const double time0 = 0.0;
  const double time1 = 0.5;

  // Camera
  Color background(0, 0, 0);
  Point3 lookfrom(478, 278, -600);
  Point3 lookat(278, 278, 0);
  double vfov = 40.0;
  Vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.0;
  Camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus,
             time0, time1);

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
    std::cout << "\rFinished line " << lines << " of " << image_height
              << " lines" << std::flush;
    lines_mutex.unlock();
  });
  std::cout << std::endl;

  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> ms_double = t2 - t1;
  std::cerr << "Time taken: " << ms_double.count() << " ms" << std::endl;

  std::cerr << "Saving file..." << std::endl;
  lodepng::encode(FILE_NAME, img.data(), image_width, image_height, LCT_RGB);
  std::cerr << "Done.\n";
  return EXIT_SUCCESS;
}
