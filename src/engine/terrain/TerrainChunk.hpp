#pragma once

#include "../gl/mesh/meshes.hpp"
#include "../../global.hpp"

struct TerrainChunk : Mesh {
  TerrainChunk() = default;

  TerrainChunk(size_t resolution, vec3 pos, float size)
    : Mesh(meshes::plane(resolution, GL_PATCHES, global::up, false)) {
    translate(pos);
    setScale({size * 0.5f, 1.f, size * 0.5f});
  }
};

