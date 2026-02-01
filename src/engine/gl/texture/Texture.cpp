#include "Texture.hpp"
#include "utils/utils.hpp"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <string>
#include <vector>

Texture::Texture(const fspath& path, const TextureDescriptor& d) : desc(d) {
  switch (desc.target) {
    case GL_TEXTURE_2D:
      create2D(image2D(path));
      break;
    case GL_TEXTURE_2D_ARRAY:
      create2DArray(path);
      break;
    default:
      error("[Texture::Texture] Unhandled texture creation type: [{}]", desc.target);
      break;
  }
}

Texture::Texture(const image2D& img, const TextureDescriptor& d) : desc(d) {
  if (desc.target != GL_TEXTURE_2D)
    error("[Texture::Texture] Unhandled texture creation type: [{}]", desc.target);

  create2D(img);
}

Texture::Texture(const Texture& other) :
  desc(other.desc),
  id(other.id) {}

void Texture::operator=(const Texture& other) {
  desc = other.desc;
  id = other.id;
}

void Texture::update(u8* pixels, ivec2 size, GLenum format) {
  bind();
  glTexSubImage2D(desc.target, 0, 0, 0, 0, size.x, size.y, format, pixels);
  unbind();
}

void Texture::bind() const {
  glActiveTexture(GL_TEXTURE0 + desc.unit);
  glBindTexture(desc.target, id);
}

void Texture::unbind() const {
  glBindTexture(desc.target, 0);
}

void Texture::clear() {
  if (id) glDeleteTextures(1, &id);
  id = 0;
}

const GLuint& Texture::getId() const { return id; }
const GLenum& Texture::getTarget() const { return desc.target; }
const GLuint& Texture::getUnit() const { return desc.unit; }
const std::string& Texture::getUniformName() const { return desc.uniformName; }
// const ivec2& Texture::getSize() const { return desc.size; }

void Texture::setUnit(GLuint unit) {
  this->desc.unit = unit;
}

void Texture::setUniformName(const std::string& name) {
  desc.uniformName = name;
}

void Texture::create2D(const image2D& img) {
  assert(desc.target == GL_TEXTURE_2D);

  glGenTextures(1, &id);
  bind();
  glTexParameteri(desc.target, GL_TEXTURE_MIN_FILTER, desc.minFilter);
  glTexParameteri(desc.target, GL_TEXTURE_MAG_FILTER, desc.magFilter);
  glTexParameteri(desc.target, GL_TEXTURE_WRAP_S, desc.wrapS);
  glTexParameteri(desc.target, GL_TEXTURE_WRAP_T, desc.wrapT);
  glTexImage2D(desc.target, 0, desc.internalFormat, img.width, img.height, 0, desc.format, desc.type, img.pixels);
  unbind();
}

void Texture::create2DArray(const fspath& folder) {
  assert(desc.target == GL_TEXTURE_2D_ARRAY);

  std::vector<fspath> paths;
  for (const auto& entry : std::filesystem::directory_iterator(folder))
    paths.push_back(entry.path());

  std::ranges::sort(paths);

  image2D img0(paths[0]);

  glGenTextures(1, &id);
  bind();
  glTexParameteri(desc.target, GL_TEXTURE_MIN_FILTER, desc.minFilter);
  glTexParameteri(desc.target, GL_TEXTURE_MAG_FILTER, desc.magFilter);
  glTexParameteri(desc.target, GL_TEXTURE_WRAP_S, desc.wrapS);
  glTexParameteri(desc.target, GL_TEXTURE_WRAP_T, desc.wrapT);
  glTexStorage3D(desc.target, 1, desc.internalFormat, img0.width, img0.height, paths.size());
  glTexSubImage3D(desc.target, 0, 0, 0, 0, img0.width, img0.height, 1, desc.format, desc.type, img0.pixels);

  for (size_t i = 1; i < paths.size(); i++) {
    image2D img(paths[i]);
    assert(img0.width == img.width);
    assert(img0.height == img.height);
    assert(img0.channels == img.channels);

    glTexSubImage3D(desc.target, 0, 0, 0, i, img.width, img.height, 1, desc.format, desc.type, img.pixels);
  }

  unbind();
}

