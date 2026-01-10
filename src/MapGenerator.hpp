#pragma once

#include <algorithm>
#include <cmath>

#include "gl/texture/Texture.hpp"
#include "gl/texture/image2D.hpp"
#include "noise.hpp"

struct MapGenerator {
  static Texture gen(ivec2 size, float scale) {
    float invScale = 1.f / std::max(scale, 0.0001f);
    u8* pixels = new u8[size.x * size.y * 4];

    for (int y = 0; y < size.y; y++)
      for (int x = 0; x < size.x; x++) {
        int yy = y << 2;
        float p = noise::perlin({x * invScale, y * invScale});
        pixels[yy + 0 + x * size.y * 4] = std::lerp(0, 255, p);
        pixels[yy + 1 + x * size.y * 4] = std::lerp(0, 255, p);
        pixels[yy + 2 + x * size.y * 4] = std::lerp(0, 255, p);
        pixels[yy + 3 + x * size.y * 4] = std::lerp(0, 255, p);
      }

    image2D img;
    img.width = size.x;
    img.height = size.y;
    img.pixels = pixels;

    Texture tex = Texture(img, {"u_diffuse0", size, 0, GL_TEXTURE_2D, GL_RGBA, GL_RGBA});

    delete[] pixels;
    img.pixels = nullptr;

    return tex;
  }
};

