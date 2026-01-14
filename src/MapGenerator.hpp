#pragma once

#include <algorithm>
#include <format>

#include "gl/Shader.hpp"
#include "gl/texture/Texture.hpp"
#include "gl/texture/image2D.hpp"

struct MapGenerator {
  Texture noiseTex;
  Texture terrainTex;

  ivec2 size{128, 128};
  float scale = 25.f;
  float persistance = 0.5f;
  float lacunarity = 2.f;
  int octaves = 4;
  int seed = 1;
  vec2 offset{};

  int tescDiv = 64;
  float terrainScale = 20.f;

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

  MapGenerator() {
    gen();
  }

  void gen() {
    static Shader noiseShader("noise2d.comp");
    static Shader terrainShader("terrain.comp");

    image2D mapImg;
    mapImg.width = size.x;
    mapImg.height = size.y;

    TextureDescriptor desc;
    desc.uniformName = "u_noiseTex";
    desc.size = size;
    desc.unit = 0;
    desc.target = GL_TEXTURE_2D;
    desc.internalFormat = GL_RGBA8;
    desc.minFilter = GL_NEAREST;
    desc.magFilter = GL_NEAREST;
    desc.wrapS = GL_CLAMP_TO_EDGE;
    desc.wrapT = GL_CLAMP_TO_EDGE;

    noiseTex.clear();
    noiseTex = Texture(mapImg, desc);

    desc.uniformName = "u_terrainTex";
    desc.unit = 1;
    terrainTex.clear();
    terrainTex = Texture(mapImg, desc);

    constexpr uvec2 localSize(16); // NOTE: Must match in the shader
    const uvec2 numGroups = (uvec2(size) + localSize - 1u) / localSize;

    noiseShader.setUniform1f("u_invScale", 1.f / std::max(scale, 0.01f));
    noiseShader.setUniform1f("u_persistance", persistance);
    noiseShader.setUniform1f("u_lacunarity", lacunarity);
    noiseShader.setUniform1i("u_octaves", octaves);
    noiseShader.setUniform1i("u_seed", seed);
    noiseShader.setUniform2f("u_offset", offset);

    glBindImageTexture(0, noiseTex.getId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
    glDispatchCompute(numGroups.x, numGroups.y, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    for (const Region& r : regions) {
      terrainShader.setUniform1f(std::vformat(r.uniformFmt, std::make_format_args("Height")), r.height);
      terrainShader.setUniform3f(std::vformat(r.uniformFmt, std::make_format_args("Color")), r.color);
    }

    glBindImageTexture(0, noiseTex.getId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    glBindImageTexture(1, terrainTex.getId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
    glDispatchCompute(numGroups.x, numGroups.y, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  }
};

