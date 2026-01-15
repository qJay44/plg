#pragma once

#include "Mesh.hpp"

namespace meshes {

[[nodiscard]] Mesh plane(vec3 color = {1.f, 1.f, 1.f}, GLenum mode = GL_TRIANGLES, bool clearable = true);
[[nodiscard]] Mesh plane(size_t resolution, GLenum mode = GL_TRIANGLES, bool clearable = true);
[[nodiscard]] Mesh axis(float size, bool clearable = true);

} // namespace meshes

