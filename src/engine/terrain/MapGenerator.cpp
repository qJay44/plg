#include "MapGenerator.hpp"

#include <algorithm>

#include "../gl/texture/image2D.hpp"
#include "../gl/Shader.hpp"

MapGenerator::MapGenerator(vec2 offset) : offset(offset) {
  gen();
}

void MapGenerator::gen() {
  static Shader falloffShader("falloff.comp");
  static Shader noiseShader("noise2d.comp");

  image2D mapImg;
  mapImg.width = size.x;
  mapImg.height = size.y;

  TextureDescriptor desc;
  desc.unit = 0;
  desc.target = GL_TEXTURE_2D;
  desc.internalFormat = GL_R8;
  desc.minFilter = GL_NEAREST;
  desc.magFilter = GL_NEAREST;
  desc.wrapS = GL_CLAMP_TO_EDGE;
  desc.wrapT = GL_CLAMP_TO_EDGE;

  desc.uniformName = "u_falloffTex";
  desc.unit = 0;
  falloffTex.clear();
  falloffTex = Texture(mapImg, desc);

  noiseTex.clear();
  desc.uniformName = "u_noiseTex";
  desc.unit = 1;
  noiseTex = Texture(mapImg, desc);

  constexpr uvec2 localSize(16); // NOTE: Must match in the shader
  const uvec2 numGroups = (uvec2(size) + localSize - 1u) / localSize;

  falloffShader.use();
  falloffShader.setUniform1f("a", falloffA);
  falloffShader.setUniform1f("b", falloffB);

  glBindImageTexture(0, falloffTex.getId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8);
  glDispatchCompute(numGroups.x, numGroups.y, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  noiseShader.use();
  noiseShader.setUniform1f("u_invScale", 1.f / std::max(scale, 0.01f));
  noiseShader.setUniform1f("u_persistance", persistance);
  noiseShader.setUniform1f("u_lacunarity", lacunarity);
  noiseShader.setUniform1f("u_noiseMaxReducer", noiseMaxReducer);
  noiseShader.setUniform1i("u_octaves", octaves);
  noiseShader.setUniform1i("u_seed", seed);
  noiseShader.setUniform2f("u_offset", offset);

  glBindImageTexture(1, noiseTex.getId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8);
  glDispatchCompute(numGroups.x, numGroups.y, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void MapGenerator::clear() {
  falloffTex.clear();
  noiseTex.clear();
}

