#pragma once

#include "gl/mesh/meshes.hpp"
#include "gl/Shader.hpp"
#include "gl/texture/Texture.hpp"

struct MapGenerator {
  Mesh plane = meshes::plane(20, GL_PATCHES);
  Texture noiseTex;
  Texture terrainTex;

  ivec2 size{128, 128};
  float scale = 25.f;
  float persistance = 0.5f;
  float lacunarity = 2.f;
  int octaves = 4;
  int seed = 1;
  vec2 offset{};

  float tescDiv = 64.f;
  float heightMultiplier = 12.f;
  float planeScale = 600.f;

  struct Region {
    const char* uniformFmt;
    float height;
    vec3 color;
  };

  std::array<Region, 8> regions {
    Region{"u_waterDeep{}"   , 0.30f, {0.11f, 0.27f, 0.63f}},
    Region{"u_waterShallow{}", 0.40f, {0.22f, 0.38f, 0.74f}},
    Region{"u_sand{}"        , 0.45f, {0.81f, 0.85f, 0.48f}},
    Region{"u_grass{}"       , 0.55f, {0.35f, 0.65f, 0.10f}},
    Region{"u_grass2{}"      , 0.60f, {0.25f, 0.55f, 0.09f}},
    Region{"u_rock{}"        , 0.70f, {0.35f, 0.25f, 0.24f}},
    Region{"u_rock2{}"       , 0.90f, {0.28f, 0.22f, 0.21f}},
    Region{"u_snow{}"        , 1.00f, {1.00f, 1.00f, 1.00f}},
  };

  MapGenerator(vec3 pos = vec3(0.f));

  void gen();
  void draw(const Camera* camera, const Shader& shader, bool forceNoWireframe = false) const;
};

