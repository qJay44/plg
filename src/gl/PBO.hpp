#pragma once

// Pixel Buffer Object
struct PBO {
  GLuint id = 0;
  GLsizei size = 0;

  PBO() {}

  PBO(GLsizei size, const void* data, GLsizeiptr dataSize, GLenum usage) : size(size) {
    glGenBuffers(size, &id);
    bind();
    glBufferData(GL_PIXEL_PACK_BUFFER, dataSize, data, usage);
    unbind();
  }

  static void unbind() { glBindBuffer(GL_PIXEL_PACK_BUFFER, 0); }

  void bind()   const { glBindBuffer(GL_PIXEL_PACK_BUFFER, id); }
  void clear()        {
    if (id) glDeleteBuffers(size, &id);
    id = 0;
    size = 0;
  }
};

