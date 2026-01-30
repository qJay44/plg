#include "Terrain.hpp"

#include <cassert>
#include <cmath>
#include <format>
#include <fstream>
#include <sstream>
#include <string>

#include "MapGenerator.hpp"
#include "../../colormaps/jet.hpp"
#include "glm/common.hpp"
#include "utils/utils.hpp"

static const fspath regionsPath = "res/regions";
constexpr const char* csvFormat = "cx,cy,cz,h";

Terrain::Terrain(vec3 pos) {
  size_t dataSize = sharedMapGen.size.x * sharedMapGen.size.y * sizeof(float);
  pbos[0] = PBO(1, nullptr, dataSize, GL_STREAM_READ);
  pbos[1] = PBO(1, nullptr, dataSize, GL_STREAM_READ);

  update(pos, true);
}

void Terrain::update(const vec3& pos, bool force) {
  vec2 posOnTerrainGlobal{pos.x, pos.z};
  ivec2 currChunkMiddleCoord = glm::floor(posOnTerrainGlobal / chunkSize);

  if (autoChunkSize)
    chunkSize = sharedMapGen.size.x * 0.5f;

  if ((chunkMiddleCoord != currChunkMiddleCoord && attachCam) || force) {
    sharedMapGen.offset = currChunkMiddleCoord * sharedMapGen.size.x;
    sharedMapGen.offset /= chunksPerAxis;
    sharedMapGen.offset += offset;
    sharedMapGen.offset.y *= -1.f;
    sharedMapGen.gen();
    build(currChunkMiddleCoord);
  }
}

void Terrain::loadRegions(std::string_view name) {
  fspath regionsFilePath = regionsPath / name;

  std::ifstream f(regionsFilePath);

  if (f.is_open()) {
    std::string line;
    f >> line;

    if (line != csvFormat)
      error("[Terrain::loadRegions] Got wrong format [{}]", line);

    for (int i = 0; f >> line; i++) {
      std::istringstream iss(line);
      std::string valueStr;
      vec4 ch;

      for (int j = 0; std::getline(iss, valueStr, ','); j++) {
        assert(j < 4);
        ch[j] = std::stof(valueStr);
      }

      assert(i < TERRAIN_REGIONS);
      colors[i] = vec3(ch);
      heights[i] = ch.a;
      regions = i + 1;
    }
  } else {
    error("[Terrain::loadRegions] Could not open the file [{}]", regionsFilePath.string());
  }

}

void Terrain::saveRegions(std::string_view name) const {
  std::filesystem::create_directories(regionsPath);

  fspath regionsFilePath = regionsPath / name;
  std::ofstream f(regionsFilePath);

  if (f.is_open()) {
    f << csvFormat << "\n";
    for (int i = 0; i < regions; i++) {
      const vec3& c = colors[i];
      const float& h = heights[i];
      f << std::format("{},{},{},{}\n", c.x, c.y, c.z, h);
    }
    f.close();
  } else {
    error("[Terrain::saveRegions] Could not open the file [{}]", regionsFilePath.string());
  }
}

float Terrain::getHeightAt(const vec3& pos) {
  if (!attachCam)
    return -9e9f;

  float terrainSize = chunksPerAxis * chunkSize;
  vec2 coord00f(chunkMiddleCoord - chunksFromMiddle);
  vec2 posOnTerrainGlobal{pos.x, pos.z};
  vec2 posOnTerrainLocal = posOnTerrainGlobal - coord00f * chunkSize;
  vec2 posOnTerrainUV = posOnTerrainLocal / terrainSize;
  posOnTerrainUV.y = 1.f - posOnTerrainUV.y;
  ivec2 offset = ivec2(posOnTerrainUV * float(sharedMapGen.size.x));

  pbos[writeIdx].bind();
  glGetTextureSubImage(sharedMapGen.noiseTex.getId(), 0, offset.x, offset.y, 0, 1, 1, 1, GL_RED, GL_FLOAT, sizeof(float), 0);

  pbos[readIdx].bind();
  float* ptr = (float*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, sizeof(float), GL_MAP_READ_BIT);

  if (!ptr)
    error("[Terrain::getHeightAt] glMapBufferRange nullptr");

  float height = calcHeight(*ptr);

  glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
  PBO::unbind();
  readIdx = !readIdx;
  writeIdx = !writeIdx;

  return height;
}

void Terrain::draw(const Camera* camera, Shader& shader, bool forceNoWireframe) const {
  shader.setUniform1i("u_div", tescDiv);
  shader.setUniform1i("u_terrainRegions", regions);
  shader.setUniform1f("u_heightMultiplier", heightMultiplier);
  shader.setUniform1f("u_maskDebugColors", showChunks);
  shader.setUniform1f("u_maskNormalmap", showChunkNormals);
  shader.setUniform1f("u_maskFalloffmap", useFalloffmap);
  shader.setUniform1f("u_minHeight", calcHeight(0.f));
  shader.setUniform1f("u_maxHeight", calcHeight(1.f));
  shader.setUniform2f("u_chunks", vec2(chunksPerAxis));
  shader.setUniform1fv("u_terrainHeights", regions, (GLfloat*)heights.data());
  shader.setUniform3fv("u_terrainColors", regions, (GLfloat*)colors.data());
  shader.setUniformTexture(sharedMapGen.noiseTex);

  sharedMapGen.noiseTex.bind();

  vec2 chunkOffsetStep(1.f / chunksPerAxis);

  for (int i = 0; i < chunksPerAxis; i++) {
    for (int j = 0; j < chunksPerAxis; j++) {
      int idx = j + i * chunksPerAxis;
      const TerrainChunk& tc = chunks[idx];

      shader.setUniform2f("u_chunkOffset", vec2(j, i) * chunkOffsetStep);
      shader.setUniform3f("u_chunkDebugColor", colormaps::jet[idx % colormaps::jet.size()]);

      tc.draw(camera, shader, forceNoWireframe);
    }
  }

  sharedMapGen.noiseTex.unbind();
}

void Terrain::build(ivec2 middleCoord) {
  chunksTotal = chunksPerAxis * chunksPerAxis;
  chunkMiddleCoord = middleCoord;
  chunksFromMiddle = chunksPerAxis / 2;

  if (chunks.size() != chunksTotal)
    chunks.resize(chunksTotal);

  for (int i = 0; i < chunksPerAxis; i++) {
    for (int j = 0; j < chunksPerAxis; j++) {
      TerrainChunk& tc = chunks[j + i * chunksPerAxis];

      ivec2 coord = ivec2{j, chunksPerAxis - 1 - i};
      vec2 chunkCoord = vec2(chunkMiddleCoord + coord - chunksFromMiddle);
      vec2 chunkPos = chunkCoord * chunkSize;
      vec2 chunkCenterPos = chunkPos + chunkSize * 0.5f; // center of a chunk

      tc.clear();
      tc = TerrainChunk(chunkResolution, {chunkCenterPos.x, 0.f, chunkCenterPos.y}, chunkSize);
    }
  }
}

float Terrain::calcHeight(float val) const {
  return std::exp(val * heightMultiplier);
}

