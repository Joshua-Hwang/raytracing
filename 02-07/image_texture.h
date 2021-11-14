#include "../external/lodepng/lodepng.h"
#include "color.h"
#include "perlin.h"
#include "rtweekend.h"
#include "texture.h"

#include <iostream>

class ImageTexture final : public Texture {
public:
  const static int bytes_per_pixel = 3;

  ImageTexture() : data(), width(0), height(0), bytes_per_scanline(0) {}

  ImageTexture(const char *filename) {
    if (lodepng::decode(data, width, height, filename,
                        LodePNGColorType::LCT_RGB) != 0) {
      std::cerr << "ERROR: Could not load texture image file '" << filename
                << "'.\n";
      width = height = 0;
    }

    bytes_per_scanline = bytes_per_pixel * width;
  }

  virtual Color value(double u, double v, const Vec3 &p) const override {
    // If we have no texture data, then return solid cyan as a debugging aid
    if (width == 0 || height == 0)
      return Color(0, 1, 1);

    // Clamp input texture coordinates to [0,1] x [1,0]
    u = clamp(u, 0.0, 1.0);
    v = 1.0 - clamp(v, 0.0, 1.0); // Flip v to image coords

    auto i = static_cast<int>(u * width);
    auto j = static_cast<int>(v * height);

    if (i >= width)
      i = width - 1;
    if (j >= height)
      j = height - 1;

    const auto color_scale = 1.0 / 255.0;
    auto pixel = data.data() + j * bytes_per_scanline + i * bytes_per_pixel;

    return Color(color_scale * pixel[0], color_scale * pixel[1],
                 color_scale * pixel[2]);
  }

private:
  std::vector<unsigned char> data;
  unsigned width, height;
  unsigned bytes_per_scanline;
};
