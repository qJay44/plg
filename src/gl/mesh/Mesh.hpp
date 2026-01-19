#pragma once

#include <vector>

#include "Vertex.hpp"
#include "Transformable.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "../Camera.hpp"
#include "../Shader.hpp"

class Mesh : public Transformable {
public:
  Mesh() = default;
  Mesh(const std::vector<Vertex4>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool clearable);
  Mesh(const std::vector<VertexPT>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool clearable);
  ~Mesh();

  void clear();
  void draw(const Camera* camera, const Shader& shader, bool forceNoWireframe = false) const;

private:
  GLsizei count = 0;
  GLenum mode;
  bool autoClear;
  VAO vao;
  VBO vbo;
  EBO ebo;
};

