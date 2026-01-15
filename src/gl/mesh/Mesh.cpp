#include "Mesh.hpp"

Mesh::Mesh(const std::vector<Vertex4>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool autoClear)
  : count(indices.size()),
    mode(mode),
    autoClear(autoClear),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(vertices[0]) * vertices.size())),
    ebo(1, indices.data(), sizeof(GLuint) * indices.size())
{
  vao.bind();
  vbo.bind();
  ebo.bind();

  size_t typeSize = sizeof(float);
  GLsizei stride = sizeof(vertices[0]);

  vao.linkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vao.linkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize));
  vao.linkAttrib(2, 2, GL_FLOAT, stride, (void*)(6 * typeSize));
  vao.linkAttrib(3, 3, GL_FLOAT, stride, (void*)(8 * typeSize));

  VAO::unbind();
  VBO::unbind();
  EBO::unbind();
}

Mesh::Mesh(const std::vector<VertexPT>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool autoClear)
  : count(indices.size()),
    mode(mode),
    autoClear(autoClear),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(vertices[0]) * vertices.size())),
    ebo(1, indices.data(), sizeof(GLuint) * indices.size())
{
  vao.bind();
  vbo.bind();
  ebo.bind();

  size_t typeSize = sizeof(float);
  GLsizei stride = sizeof(vertices[0]);

  vao.linkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vao.linkAttrib(1, 2, GL_FLOAT, stride, (void*)(3 * typeSize));

  VAO::unbind();
  VBO::unbind();
  EBO::unbind();
}

Mesh::~Mesh() {
  if (autoClear)
    clear();
};

void Mesh::clear() {
  if (vao.size) vao.clear();
  if (vbo.size) vbo.clear();
  if (ebo.size) ebo.clear();
}

void Mesh::draw(const Camera* camera, const Shader& shader, bool forceNoWireframe) const {
  vao.bind();

  mat4 model = transMat * rotMat * scaleMat;

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

  glDrawElements(mode, count, GL_UNSIGNED_INT, 0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  vao.unbind();
}

