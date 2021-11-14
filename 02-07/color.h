#pragma once

#include "rtweekend.h"
#include "vec3.h"

#include <iostream>
#include <utility>
#include <vector>

class Image {
public:
  Image(std::size_t width, std::size_t height) : width(width), height(height) {
    img.resize(width * height * 3);
  }

  Color operator[](std::pair<unsigned, unsigned> p) {
    const auto [x, y] = p;
    const auto r = img[3 * (width * y + x) + 0];
    const auto g = img[3 * (width * y + x) + 1];
    const auto b = img[3 * (width * y + x) + 2];

    return Color(r, g, b);
  }

  void set(std::pair<std::size_t, std::size_t> p, Color color) {
    auto [x, y] = p;
    img[3 * (width * y + x) + 0] =
        static_cast<unsigned char>(256 * clamp(color[0], 0.0, 0.999));
    img[3 * (width * y + x) + 1] =
        static_cast<unsigned char>(256 * clamp(color[1], 0.0, 0.999));
    img[3 * (width * y + x) + 2] =
        static_cast<unsigned char>(256 * clamp(color[2], 0.0, 0.999));
  }

  auto data() { return img.data(); }

  const std::size_t width;
  const std::size_t height;

private:
  std::vector<unsigned char> img;
};
