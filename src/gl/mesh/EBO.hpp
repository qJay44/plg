#pragma once

// Element Buffer Object
struct EBO {
  GLuint id = 0;
  GLsizei size = 0;

  EBO() {}

  EBO(GLsizei size, const void* data, GLsizeiptr dataSize) : size(size) {
    glGenBuffers(size, &id);
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
    unbind();
  }

  static void unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

  void bind()   const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); }
  void clear()        {
    if (id) glDeleteBuffers(size, &id);
    id = 0;
    size = 0;
  }
};

