#pragma once

#include "TerrainChunk.hpp"

class Terrain {
public:
  Terrain(vec3 pos);

  void update(vec3 pos, bool force = false);
  void draw(const Camera* camera, const Shader& shader, bool forceNoWireframe = false) const;

private:
  friend struct gui;
  vec2 temp{1.f};

  TerrainChunk chunks[3][3];
  int chunkResolution = 5;
  float chunkSize = 64.f;
  ivec2 chunk00Coord{};
  MapGenerator sharedMapGen{};

  bool useDebugColors = false;
  bool attachCam = true;
  bool autoChunkSize = true;

private:
  void build(ivec2 coord00);
};

