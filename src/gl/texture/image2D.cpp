#include "image2D.hpp"

#include <format>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "utils/status.hpp"

void image2D::write(const std::string& path, uvec2 size, u8 channels, byte* buf) {
  stbi_flip_vertically_on_write(true);
  stbi_write_png(path.c_str(), size.x, size.y, channels, buf, size.x * channels);
}

image2D::image2D() {}

image2D::image2D(const fspath& path, bool flipVertically) { load(path, flipVertically); }

image2D::~image2D() {
  if (stbiLoad)
    stbi_image_free(pixels);
  else if(tifInt16Load)
    delete[] (s16*)pixels;
  else if (pixels)
    error("image2D is not nullptr");
}

void image2D::load(const fspath& path, bool flipVertically) {
  status::start("Loading", path.string());

  stbi_set_flip_vertically_on_load(flipVertically);
  pixels = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
  if (!pixels) {
    status::end(false);
    error(std::format("stb can't load the image: {}", path.string()));
  }

  name = path.string();
  stbiLoad = true;

  status::end(true);
}

