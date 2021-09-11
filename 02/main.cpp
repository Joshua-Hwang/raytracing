#include "vec3.h"
#include "color.h"
#include "../external/lodepng/lodepng.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#define FILE_NAME "out.png"

int main(int argc, char *argv[]) {
  const unsigned image_width = 256;
  const unsigned image_height = 256;
  // 3 for RGB
  Image img(image_width, image_height);
  for (int y = 0; y < image_height; ++y) {
    std::cerr << "\rScanlines remaining: " << y << ' ' << std::flush;
    for (int x = 0; x < image_width; ++x) {
      Color pixel_color(double(x) / (image_width - 1), double(y) / (image_height - 1), 0.25);
      img.set({x,y}, pixel_color);
    }
  }
  std::cerr << "\nSaving file...\n";
  lodepng::encode(FILE_NAME, img.data(), image_width, image_height, LCT_RGB);
  std::cerr << "Done.\n";
  return EXIT_SUCCESS;
}
