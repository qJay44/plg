#pragma once

#include "Mesh.hpp"

namespace meshes {

[[nodiscard]] Mesh plane(vec3 position, vec2 size, vec3 color = {1.f, 1.f, 1.f}, bool clearable = true);
[[nodiscard]] Mesh axis(float size, bool clearable = true);

} // namespace meshes

