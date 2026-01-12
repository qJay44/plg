#pragma once

#include <algorithm>

#include "gl/Shader.hpp"
#include "gl/texture/Texture.hpp"
#include "gl/texture/image2D.hpp"

struct MapGenerator {
  Texture tex;

  ivec2 size{512, 512};
  float scale = 27.f;
  float persistance = 0.5f;
  float lacunarity = 2.f;
  int octaves = 4;
  int seed = 1;
  vec2 offset{};

  MapGenerator() {
    gen();
  }

  void gen() {
    static Shader noiseShader("noise2d.comp");

    image2D mapImg;
    mapImg.width = size.x;
    mapImg.height = size.y;
    mapImg.channels = 3;

    TextureDescriptor desc;
    desc.uniformName = "u_noiseTex";
    desc.size = size;
    desc.unit = 1;
    desc.target = GL_TEXTURE_2D;
    desc.internalFormat = GL_RGBA8;
    desc.minFilter = GL_NEAREST;
    desc.magFilter = GL_NEAREST;

    tex.clear();
    tex = Texture(mapImg, desc);

    constexpr uvec2 localSize(16); // NOTE: Must match in the shader
    const uvec2 numGroups = (uvec2(size) + localSize - 1u) / localSize;

    noiseShader.setUniform1f("u_invScale", 1.f / std::max(scale, 0.01f));
    noiseShader.setUniform1f("u_persistance", persistance);
    noiseShader.setUniform1f("u_lacunarity", lacunarity);
    noiseShader.setUniform1i("u_octaves", octaves);
    noiseShader.setUniform1i("u_seed", seed);
    noiseShader.setUniform2f("u_offset", offset);

    glBindImageTexture(1, tex.getId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    glDispatchCompute(numGroups.x, numGroups.y, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  }
};

