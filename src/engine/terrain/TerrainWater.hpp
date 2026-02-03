#pragma once

#include "../gl/mesh/Mesh.hpp"

class TerrainWater : public Mesh {
public:
  // NOTE: Make sure passed resolutions aspect ratio are equal to window aspect ratio
  TerrainWater(const class Terrain* terrainPtr, float heightNorm, ivec2 reflRes, ivec2 refrRes);

  void update();
  void draw(const Camera* camera, Shader& shader, bool forceNoWireframe = false) const;

  const ivec2& getReflRes() const;
  const ivec2& getRefrRes() const;

  const Texture& getReflTex() const;
  const Texture& getRefrTex() const;

  const float& getHeight() const;
  const float& getHeightNorm() const;

private:
  friend class Terrain;
  friend struct gui;

  const class Terrain* terrainPtr;
  float heightNorm;
  ivec2 reflRes;
  ivec2 refrRes;

  static Texture dudvMapTex;
  static Texture normalMapTex;

  Texture reflectionTex;
  Texture refractionTex;

  float height;
  float waveStrength = 0.3f;
  float waveFreq = 0.02f;
  float shoreFadeDist = 5.f;
};

