#pragma once

#include "vec3.h"
#include "rtweekend.h"

#include <iostream>
#include <utility>
#include <vector>

class Image {
  public:
    Image(unsigned width, unsigned height): width(width), height(height) {
      img.resize(width * height * 3);
    }

    Color operator[](std::pair<unsigned, unsigned> p) {
      const auto [x, y] = p;
      const auto r = img[3 * width * y + 3 * x + 0];
      const auto g = img[3 * width * y + 3 * x + 1];
      const auto b = img[3 * width * y + 3 * x + 2];

      return Color(r,g,b);
    }

    void set(std::pair<unsigned, unsigned> p, Color color) {
      const auto [x, y] = p;
      img[3 * width * y + 3 * x + 0] = static_cast<unsigned char>(256 * clamp(color[0], 0.0, 0.999));
      img[3 * width * y + 3 * x + 1] = static_cast<unsigned char>(256 * clamp(color[1], 0.0, 0.999));
      img[3 * width * y + 3 * x + 2] = static_cast<unsigned char>(256 * clamp(color[2], 0.0, 0.999));
    }

    auto data() {
      return img.data();
    }
  private:
    unsigned width;
    unsigned height;
    std::vector<unsigned char> img;
};
