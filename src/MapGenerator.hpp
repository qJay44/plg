#pragma once

#include "gl/texture/Texture.hpp"

struct MapGenerator {
  Texture noiseTex{};
  Texture terrainTex{};
  Texture falloffTex{};

  float falloffA = 3.f;
  float falloffB = 2.2f;

  ivec2 size{512};
  float scale = 125.f;
  float persistance = 0.5f;
  float lacunarity = 2.f;
  float noiseMaxReducer = 1.23f;
  int octaves = 4;
  int seed = 1;
  vec2 offset{};

  float tescDiv = 64.f;
  float heightMultiplier = 10.f;

  bool useFalloffmap = false;

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

  MapGenerator() = default;
  MapGenerator(vec2 offset);

  void gen();
  void clear();
};

