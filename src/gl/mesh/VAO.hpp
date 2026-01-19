#pragma once

// Vertex Array Object
struct VAO {
  GLuint id = 0;
  GLsizei size = 0;

  VAO() {}

  VAO(GLsizei size) : size(size) {
    glGenVertexArrays(size, &id);
  }

  static void unbind() { glBindVertexArray(0); }

  void bind()   const { glBindVertexArray(id); }
  void clear()        {
    if (id) glDeleteVertexArrays(size, &id);
    id = 0;
    size = 0;
  }

  void linkAttrib(GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset) const {
    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
  }
};

