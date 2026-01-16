#pragma once

#include "TextureDescriptor.hpp"
#include "image2D.hpp"

class Texture {
public:
  Texture() = default;

  Texture(const fspath& path, const TextureDescriptor& desc);
  Texture(const image2D& img, const TextureDescriptor& desc);

  Texture(const Texture& other);

  void operator=(const Texture& other);

  void update(u8* pixels, ivec2 size, GLenum format);
  void bind() const;
  void unbind() const;
  void clear();

  const GLuint& getId() const;
  const GLenum& getTarget() const;
  const GLuint& getUnit() const;
  const std::string& getUniformName() const;
  // const ivec2& getSize() const;

  void setUnit(GLuint unit);
  void setUniformName(const std::string& name);

private:
  TextureDescriptor desc;
  GLuint id = 0;

private:
  void create(const image2D& img);
};

