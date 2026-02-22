#include "TerrainWater.hpp"

#include "../gl/mesh/meshes.hpp"

#include "Terrain.hpp"

Texture TerrainWater::dudvMapTex;
Texture TerrainWater::normalMapTex;

TerrainWater::TerrainWater(
  const Terrain* terrainPtr,
  float heightNorm,
  ivec2 reflRes,
  ivec2 refrRes
)
  : Mesh(meshes::plane(2)),
    terrainPtr(terrainPtr),
    heightNorm(heightNorm),
    reflRes(reflRes),
    refrRes(refrRes)
{
  if (dudvMapTex.getUniformName().empty()) {
    TextureDescriptor desc;
    desc.uniformName = "u_dudvMapTex";
    desc.unit = 2;
    desc.internalFormat = GL_RG;
    desc.minFilter = GL_LINEAR;
    desc.magFilter = GL_LINEAR;
    desc.wrapS = GL_REPEAT;
    desc.wrapT = GL_REPEAT;
    desc.genMipMap = false;

    dudvMapTex = Texture(image2D("res/tex/water/dudv.png"), desc);

    desc.uniformName = "u_normalMapTex";
    desc.unit = 5;
    desc.internalFormat = GL_RGB;
    normalMapTex = Texture(image2D("res/tex/water/normal.png"), desc);
  }

  TextureDescriptor desc;
  desc.uniformName = "u_reflectionTex";
  desc.minFilter = GL_LINEAR;
  desc.magFilter = GL_LINEAR;
  desc.wrapS = GL_REPEAT;
  desc.wrapT = GL_REPEAT;
  desc.genMipMap = false;

  reflectionTex = Texture({reflRes.x, reflRes.y}, desc);

  desc.uniformName = "u_refractionTex";
  desc.unit = 1;
  refractionTex = Texture({refrRes.x, refrRes.y}, desc);

  update();
}

void TerrainWater::update() {
  float waterPlaneScale = terrainPtr->getSize() * 0.5f;
  height = terrainPtr->calcHeight(heightNorm);

  setScale({waterPlaneScale, 0.f, waterPlaneScale});
  setTrans(terrainPtr->getMiddlePos() + vec3{0.f, height, 0.f});
}

void TerrainWater::draw(const Camera* camera, Shader& shader, bool forceNoWireframe) const {
  shader.setUniform1f("u_time", global::time);
  shader.setUniform1f("u_waveStrength", waveStrength);
  shader.setUniform1f("u_waveFreq", waveFreq);
  shader.setUniform1f("u_tiling", tiling);
  shader.setUniform1f("u_specularDamper", specularDamper);
  shader.setUniform1f("u_specularStrength", specularStrength);

  reflectionTex.bind();
  refractionTex.bind();
  dudvMapTex.bind();
  normalMapTex.bind();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Mesh::draw(camera, shader, forceNoWireframe);

  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  reflectionTex.unbind();
  refractionTex.unbind();
  dudvMapTex.unbind();
  normalMapTex.unbind();
}

const ivec2& TerrainWater::getReflRes() const { return reflRes; }
const ivec2& TerrainWater::getRefrRes() const { return refrRes; }

const Texture& TerrainWater::getReflTex() const { return reflectionTex; }
const Texture& TerrainWater::getRefrTex() const { return refractionTex; }

const float& TerrainWater::getHeight()     const { return height;     }
const float& TerrainWater::getHeightNorm() const { return heightNorm; }

