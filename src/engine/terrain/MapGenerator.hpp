#pragma once

#include "../gl/texture/Texture.hpp"

struct MapGenerator {
  Texture noiseTex{};
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

  MapGenerator() = default;
  MapGenerator(vec2 offset);

  void gen();
  void clear();
};

