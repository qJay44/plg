#include "meshes.hpp"

#include <vector>

namespace meshes {

Mesh plane(vec3 pos, vec2 size, vec3 color, bool clearable) {
  std::vector<Vertex4> vertices{
    {{-0.1f, -0.1f, 0.f}, color, {0.f, 0.f}, {-1.f, 0.f, 0.f}},
    {{-0.1f,  0.1f, 0.f}, color, {0.f, 1.f}, {-1.f, 0.f, 0.f}},
    {{ 0.1f,  0.1f, 0.f}, color, {1.f, 1.f}, {-1.f, 0.f, 0.f}},
    {{ 0.1f, -0.1f, 0.f}, color, {1.f, 0.f}, {-1.f, 0.f, 0.f}},
  };

  std::vector<GLuint> indices{
    0, 1, 2,
    2, 3, 0
  };

  Mesh m = Mesh(vertices, indices, GL_TRIANGLES, clearable);
  m.translate(pos);
  m.scale(size);

  return m;
}

Mesh axis(float size, bool clearable) {
  std::vector<Vertex4> vertices{
    {{0.f, 0.f, 0.f} , global::red},
    {{size, 0.f, 0.f}, global::red},
    {{0.f, 0.f, 0.f} , global::green},
    {{0.f, size, 0.f}, global::green},
    {{0.f, 0.f, 0.f} , global::blue},
    {{0.f, 0.f, size}, global::blue},
  };

  std::vector<GLuint> indices {
    0, 1,
    2, 3,
    4, 5
  };

  return Mesh(vertices, indices, GL_LINES, clearable);
}

} // namespace meshes
