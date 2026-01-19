#pragma once

#include "TerrainChunk.hpp"

class Terrain {
public:
  Terrain(vec3 pos);

  void update(vec3 pos, bool force = false);
  void draw(const Camera* camera, const Shader& shader, bool forceNoWireframe = false) const;

private:
  friend struct gui;

  TerrainChunk chunks[3][3];
  int chunkResolution = 5;
  float chunkSize = 64.f;
  bool useDebugColors = false;
  bool attachCam = true;
  ivec2 chunk00Coord{};
  MapGenerator sharedMapGen{};

private:
  void build(ivec2 coord00);
};

