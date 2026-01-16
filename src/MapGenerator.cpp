#include "MapGenerator.hpp"

#include <algorithm>
#include <format>

#include "gl/texture/image2D.hpp"

MapGenerator::MapGenerator(vec3 pos) {
  plane.translate(pos);
  plane.setScale({planeScale, 1.f, planeScale});
  gen();
}

void MapGenerator::gen() {
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

void MapGenerator::update() {
  vec3 camPos = global::camera->getPosition();
  gen();
  offset.x = camPos.x;
  offset.y = -camPos.z;

  camPos.y = 0.f;
  plane.setTrans(camPos);
}

void MapGenerator::draw(const Camera* camera, const Shader& shader, bool forceNoWireframe) const {
  terrainTex.bind();
  noiseTex.bind();
  plane.draw(camera, shader, forceNoWireframe);
  noiseTex.bind();
  terrainTex.unbind();
}

