#pragma once

#include "MapGenerator.hpp"
#include "TerrainChunk.hpp"
#include "../gl/PBO.hpp"

class Terrain {
public:
  Terrain(vec3 pos);

  void update(const vec3& pos, bool force = false);
  float getHeightAt(const vec3& pos);
  void draw(const Camera* camera, Shader& shader, bool forceNoWireframe = false) const;

private:
  friend struct gui;

  int chunksPerAxis = 3;
  size_t chunksTotal = chunksPerAxis * chunksPerAxis;
  int chunkResolution = 5;
  int chunksFromMiddle = chunksPerAxis / 2; // Offset to build around camera (centered)
  float chunkSize = 64.f;
  ivec2 chunkMiddleCoord{};
  std::vector<TerrainChunk> chunks;

  MapGenerator sharedMapGen{};

  PBO pbos[2];
  bool readIdx = false; // 0
  bool writeIdx = true; // 1

  bool showChunks = false;
  bool showChunkNormals = false;
  bool attachCam = true;
  bool autoChunkSize = true;

private:
  void build(ivec2 middleCoord);
};

