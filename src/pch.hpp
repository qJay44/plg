#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::uvec2;
using glm::uvec3;
using glm::uvec4;

using glm::ivec2;
using glm::ivec3;
using glm::ivec4;

using glm::dvec2;
using glm::dvec3;
using glm::dvec4;

using glm::mat3;
using glm::mat4;

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#define UTILS_ENABLE_GLM
#include "utils/utils.hpp"
#include "utils/types.hpp"

#include "defines.hpp"

#define IM_VEC2_CLASS_EXTRA                                             \
  constexpr ImVec2(const vec2& f) : x(f.x), y(f.y) {}                   \
  operator vec2() const { return vec2(x,y); }                           \
                                                                        \
  ImVec2 operator+(const ImVec2& rhs) const {                           \
    return ImVec2(x + rhs.x, y + rhs.y);                                \
  }

#define IM_VEC3_CLASS_EXTRA                                             \
  constexpr ImVec3(const vec3& f) : x(f.x), y(f.y), z(f.z) {}           \
  operator vec3() const { return vec3(x,y,z); }

#define IM_VEC4_CLASS_EXTRA                                             \
  constexpr ImVec4(const vec4& f) : x(f.x), y(f.y), z(f.z), w(f.w) {}   \
  operator vec4() const { return vec4(x,y,z,w); }

