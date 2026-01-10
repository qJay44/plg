#include "Mesh.hpp"

#include "glm/ext/matrix_transform.hpp"

Mesh::Mesh(const std::vector<Vertex4>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool clearable)
  : vertices(vertices),
    indices(indices),
    mode(mode),
    clearable(clearable),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex4) * vertices.size())),
    ebo(1, indices.data(), sizeof(GLuint) * indices.size())
{
  vao.bind();
  vbo.bind();
  ebo.bind();

  size_t typeSize = sizeof(float);
  GLsizei stride = static_cast<GLsizei>((3 + 3 + 2 + 3) * typeSize);

  vao.linkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vao.linkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize));
  vao.linkAttrib(2, 2, GL_FLOAT, stride, (void*)(6 * typeSize));
  vao.linkAttrib(3, 3, GL_FLOAT, stride, (void*)(8 * typeSize));

  VAO::unbind();
  VBO::unbind();
  EBO::unbind();
}

Mesh::~Mesh() {
  if (clearable)
    clear();
}

const mat4& Mesh::getTranslation() const { return translation; }
const mat4& Mesh::getRotation()    const { return rotation;    }
const mat4& Mesh::getScale()       const { return scaleMat;    }

mat4 Mesh::getModel() const {
  return translation * rotation * scaleMat;
}

u32 Mesh::getIndicesSize()  const { return indices.size();  }

void Mesh::translate(const vec3& v) { translation = glm::translate(translation, v); }

void Mesh::rotate(const float& angle, const vec3& axis) { rotation = glm::rotate(rotation, angle, axis);}
void Mesh::rotate(const glm::quat& q) { rotation = glm::mat4_cast(q) * rotation; };

void Mesh::scale(const float& s)    { scaleMat = glm::scale(scaleMat, vec3(s)); }
void Mesh::scale(const vec2& s)     { scaleMat = glm::scale(scaleMat, vec3(s, 1.f)); }

void Mesh::draw(const Camera* camera, const Shader& shader, bool forceNoWireframe) const {
  vao.bind();

  mat4 model = translation * rotation * scaleMat;

  shader.setUniform1f("u_camNear", camera->getNearPlane());
  shader.setUniform1f("u_camFar", camera->getFarPlane());
  shader.setUniform1f("u_camFov", camera->getFov());
  shader.setUniform3f("u_camPos", camera->getPosition());
  shader.setUniform3f("u_camRight", camera->getRight());
  shader.setUniform3f("u_camUp", camera->getUp());
  shader.setUniform3f("u_camForward", camera->getForward());
  shader.setUniformMatrix4f("u_cam", camera->getMatrix());
  shader.setUniformMatrix4f("u_camInv", camera->getMatrixInverse());
  shader.setUniformMatrix4f("u_model", model);

  if (global::drawWireframe & !forceNoWireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glDrawElements(mode, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  vao.unbind();
}

void Mesh::clear() {
  if (vao.size) vao.clear();
  if (vbo.size) vbo.clear();
  if (ebo.size) ebo.clear();
}

