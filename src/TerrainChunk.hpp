#pragma once

#include "MapGenerator.hpp"
#include "gl/mesh/meshes.hpp"

struct TerrainChunk : Mesh {
  MapGenerator mg{};

  TerrainChunk() = default;

  TerrainChunk(size_t resolution, vec3 pos, float size, MapGenerator mg)
    : Mesh(meshes::plane(resolution, GL_PATCHES, false)), mg(mg) {
    this->mg.gen();
    translate(pos);
    setScale({size * 0.5f, 1.f, size * 0.5f});
  }

  void clear() {
    mg.clear();
    Mesh::clear();
  }
};

