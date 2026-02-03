#pragma once

#include <array>
#include <string_view>

#include "MapGenerator.hpp"
#include "TerrainChunk.hpp"
#include "TerrainLayer.hpp"
#include "TerrainWater.hpp"

class Terrain {
public:
  Terrain(
    vec3 pos,
    const std::string& layersName,
    const TextureDescriptor& desc,
    ivec2 waterReflRes = global::getWinSize(),
    ivec2 waterRefrRes = global::getWinSize()
  );

  void updateLayers();
  void update(const vec3& pos, bool force = false);

  void loadLayers(std::string_view name);
  void saveLayers(std::string_view name) const;

  float calcHeight(float val) const;
  float getHeightAt(const vec3& pos);
  float getSize() const;
  vec3 getMiddlePos() const;

  const TerrainWater& getWater() const;

  void draw(const Camera* camera, Shader& shader, bool forceNoWireframe = false) const;
  void drawWater(const Camera* camera, Shader& shader, bool forceNoWireframe = false) const;

private:
  friend struct gui;

  int chunksPerAxis = 3;
  size_t chunksTotal = chunksPerAxis * chunksPerAxis;
  int chunkResolution = 5;
  int chunksFromMiddle = chunksPerAxis / 2; // Offset to build around camera (centered)
  float chunkSize = 64.f;
  ivec2 chunkMiddleCoord{};
  std::array<TerrainChunk, TERRAIN_MAX_CHUNKS_PER_AXIS> chunks;

  MapGenerator sharedMapGen{};
  vec2 offset{};
  float tescDiv = 64.f;
  float heightMultiplier = 4.4f;

  std::string layersName;
  Texture layersTexture;
  int layersCount = 0;
  std::array<TerrainLayer, TERRAIN_MAX_LAYERS> layers;
  BufferObject ubo{GL_UNIFORM_BUFFER};

  TerrainWater water{this, 0.5f, {800, 800}, {1200, 720}};

  BufferObject pbos[2]{GL_PIXEL_PACK_BUFFER, GL_PIXEL_PACK_BUFFER};
  bool readIdx = false; // 0
  bool writeIdx = true; // 1

  bool showChunks = false;
  bool showChunkNormalmap = false;
  bool attachCam = true;
  bool autoChunkSize = true;
  bool useFalloffmap = false;
  bool useLighting = true;

private:
  void build(ivec2 middleCoord);
};

