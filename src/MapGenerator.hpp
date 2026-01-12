#pragma once

#include <algorithm>

#include "gl/Shader.hpp"
#include "gl/texture/Texture.hpp"
#include "gl/texture/image2D.hpp"

struct MapGenerator {
  static Texture tex;

  static void gen(ivec2 size, float scale) {
    static const image2D noiseImg("res/rgba-noise-medium.png");
    static const Texture noiseTex(noiseImg, {"u_noisePermutationTex", {}, 0, GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST});
    static Shader noiseShader("noise2d.comp");

    float invScale = 1.f / std::max(scale, 0.01f);

    image2D mapImg;
    mapImg.width = size.x;
    mapImg.height = size.y;
    mapImg.channels = 4;

    tex.clear();
    tex = Texture(mapImg, {"u_diffuse0", size, 1, GL_TEXTURE_2D, GL_RGBA8});

    constexpr uvec2 localSize(16); // NOTE: Must match in the shader
    const uvec2 numGroups = (uvec2(size) + localSize - 1u) / localSize;

    noiseShader.setUniformTexture(noiseTex);
    noiseShader.setUniform1f("u_invScale", invScale);
    noiseTex.bind();
    glBindImageTexture(1, tex.getId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    glDispatchCompute(numGroups.x, numGroups.y, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    noiseTex.unbind();
  }
};

