#pragma once

#include <cstring>

// Uniform Buffer Object
struct UBO {
  GLuint id = 0;
  GLsizei size = 0;

  UBO() = default;

  UBO(GLsizei size, const void* data, GLsizeiptr dataSize) : size(size) {
    glGenBuffers(size, &id);
    bind();
    glBufferData(GL_UNIFORM_BUFFER, dataSize, data, GL_DYNAMIC_DRAW);
    unbind();
  }

  static void unbind() {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  static void unbindPoint(GLuint index) {
    glBindBufferBase(GL_UNIFORM_BUFFER, index, 0);
  }

  void bind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, id);
  }

  void bindPoint(GLuint index) const {
    glBindBufferBase(GL_UNIFORM_BUFFER, index, id);
  }

  void update(const void* data, GLsizeiptr dataSize) {
    bind();

    void* ptr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, dataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
    memcpy(ptr, data, dataSize);

    glUnmapBuffer(GL_UNIFORM_BUFFER);
    unbind();
  }

  void clear()        {
    if (id) glDeleteBuffers(size, &id);
    id = 0;
    size = 0;
  }
};

