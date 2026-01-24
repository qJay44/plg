#include "meshes.hpp"

#include <vector>

#include "utils/utils.hpp"
#include "../../../global.hpp"

namespace meshes {

Mesh plane(vec3 color, GLenum mode, bool autoClear) {
  std::vector<Vertex4> vertices{
    {{-1.f, -1.f, 0.f}, color, {0.f, 0.f}, {1.f, 0.f, 0.f}},
    {{-1.f,  1.f, 0.f}, color, {0.f, 1.f}, {1.f, 0.f, 0.f}},
    {{ 1.f,  1.f, 0.f}, color, {1.f, 1.f}, {1.f, 0.f, 0.f}},
    {{ 1.f, -1.f, 0.f}, color, {1.f, 0.f}, {1.f, 0.f, 0.f}},
  };

  std::vector<GLuint> indices;

  switch (mode) {
    case GL_TRIANGLES: {
      indices = {
        0, 1, 2,
        2, 3, 0
      };
      break;
    }
    case GL_PATCHES: {
      indices = {
        0, 1,
        2, 3,
      };
      break;
    }
    default:
      error("[meshes::plane] Unhandled mode [{}]", mode);
  }

  return Mesh(vertices, indices, mode, autoClear);
}

Mesh plane(size_t resolution, GLenum mode, bool autoClear) {
  size_t indicesPerQuad = 0;
  if      (mode == GL_TRIANGLES) indicesPerQuad = 6;
  else if (mode == GL_PATCHES)   indicesPerQuad = 4;
  else error("[meshes::plane] Unexpected mode [{}]", mode);

  std::vector<VertexPT> vertices(resolution * resolution);
  std::vector<GLuint> indices((resolution - 1) * (resolution - 1) * indicesPerQuad);
  constexpr vec3 up{0.f, 1.f, 0.f};

  vec3 axisA = vec3(up.y, up.z, up.x);
  vec3 axisB = cross(up, axisA);
  size_t triIndex = 0;

  for (size_t y = 0; y < resolution; y++) {
    float percentY = y / (resolution - 1.f);
    vec3 pY = (percentY - 0.5f) * 2.f * axisB;

    for (size_t x = 0; x < resolution; x++) {
      size_t idx = x + y * resolution;
      float percentX = x / (resolution - 1.f);
      vec3 pX = (percentX - 0.5f) * 2.f * axisA;

      VertexPT& vert = vertices[idx];
      vert.position = up + pX + pY;
      vert.texture = {percentX, percentY};

      switch (mode) {
        case GL_TRIANGLES: {
          if (x != resolution - 1 && y != resolution - 1) {
            indices[triIndex + 0] = idx + resolution;     // 2       0 -------- 1
            indices[triIndex + 1] = idx;                  // 0       |          |
            indices[triIndex + 2] = idx + 1;              // 1       |          |
            //                                                       |          |
            indices[triIndex + 3] = idx + 1;              // 1       |          |
            indices[triIndex + 4] = idx + resolution + 1; // 3       |          |
            indices[triIndex + 5] = idx + resolution;     // 2       2 -------- 3

            triIndex += 6;
          }
          break;
        }
        case GL_PATCHES: {
          if (x != resolution - 1 && y != resolution - 1) {
            indices[triIndex + 0] = idx;                  // 0
            indices[triIndex + 1] = idx + 1;              // 1
            indices[triIndex + 2] = idx + resolution + 1; // 3
            indices[triIndex + 3] = idx + resolution;     // 2

            triIndex += 4;
          }
          break;
        }
      }
    }
  }

  return Mesh(vertices, indices, mode, autoClear);
}

Mesh axis(float size, bool autoClear) {
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

  return Mesh(vertices, indices, GL_LINES, autoClear);
}

} // namespace meshes

