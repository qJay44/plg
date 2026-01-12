#include "Texture.hpp"
#include "utils/utils.hpp"

#include <cassert>

Texture::Texture(const fspath& path, const TextureDescriptor& d) : desc(d) {
  create(image2D(path));
}

Texture::Texture(const image2D& img, const TextureDescriptor& d) : desc(d) {
  create(img);
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
  if (id)
    glDeleteTextures(1, &id);
}

const GLuint& Texture::getId() const { return id; }
const GLenum& Texture::getTarget() const { return desc.target; }
const GLuint& Texture::getUnit() const { return desc.unit; }
const std::string& Texture::getUniformName() const { return desc.uniformName; }
const ivec2& Texture::getSize() const { return desc.size; }

void Texture::setUnit(GLuint unit) {
  this->desc.unit = unit;
}

void Texture::setUniformName(const std::string& name) {
  desc.uniformName = name;
}

void Texture::create(const image2D& img) {
  switch (desc.target) {
    case GL_TEXTURE_2D: {
      desc.size = ivec2(img.width, img.height);

      glGenTextures(1, &id);
      bind();
      glTexParameteri(desc.target, GL_TEXTURE_MIN_FILTER, desc.minFilter);
      glTexParameteri(desc.target, GL_TEXTURE_MAG_FILTER, desc.magFilter);
      glTexParameteri(desc.target, GL_TEXTURE_WRAP_S, desc.wrapS);
      glTexParameteri(desc.target, GL_TEXTURE_WRAP_T, desc.wrapT);
      glTexImage2D(desc.target, 0, desc.internalFormat, img.width, img.height, 0, desc.format, desc.type, img.pixels);

      break;
    }
    default:
      error("[Texture] Unhandled texture creation type: [{}]", desc.target);
      break;
  }
  unbind();
}

