#pragma once

#include <vector>

#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "Vertex.hpp"
#include "glm/gtc/quaternion.hpp"

#include "../Camera.hpp"
#include "../Shader.hpp"

class Mesh {
public:
  Mesh(const std::vector<Vertex4>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool clearable);
  ~Mesh();

  const mat4& getTranslation() const;
  const mat4& getRotation()    const;
  const mat4& getScale()       const;
  mat4 getModel() const;
  u32 getIndicesSize() const;

  void translate(const vec3& v);
  void rotate(const float& angle, const vec3& axis);
  void rotate(const glm::quat& q);
  void scale(const float& s);
  void scale(const vec2& s);

  void draw(const Camera* camera, const Shader& shader, bool forceNoWireframe = false) const;
  void clear();

private:
  std::vector<Vertex4> vertices;
  std::vector<GLuint> indices;
  GLenum mode;
  bool clearable;

  VAO vao;
  VBO vbo;
  EBO ebo;

  mat4 translation = mat4(1.f);
  mat4 rotation    = mat4(1.f);
  mat4 scaleMat    = mat4(1.f);
};

