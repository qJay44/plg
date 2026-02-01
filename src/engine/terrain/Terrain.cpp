#include "Terrain.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <string>

#include "MapGenerator.hpp"
#include "../../colormaps/jet.hpp"
#include "glm/common.hpp"
#include "utils/utils.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

Terrain::Terrain(vec3 pos, const std::string& layersName, const TextureDescriptor& desc) : layersName(layersName) {
  ubo = UBO(1, layers.data(), sizeof(TerrainLayer) * TERRAIN_LAYERS);

  size_t dataSize = sharedMapGen.size.x * sharedMapGen.size.y * sizeof(float);
  pbos[0] = PBO(1, nullptr, dataSize, GL_STREAM_READ);
  pbos[1] = PBO(1, nullptr, dataSize, GL_STREAM_READ);

  layersTexture = Texture(fspath("res/tex/layers") / layersName, desc);
  sharedMapGen.unitOffset = desc.unit + 1;

  update(pos, true);
}

void Terrain::updateLayers() {
  ubo.update(layers.data(), sizeof(TerrainLayer) * TERRAIN_LAYERS);
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

void Terrain::loadLayers(std::string_view name) {
  fspath path = fspath("res/data/layers") / layersName / name;

  std::ifstream f(path);

  if (f.is_open()) {
    json data = json::parse(f);
    layersCount = data.size();

    for (size_t i = 0; i < data.size(); i++) {
      TerrainLayer& l = layers[i];
      const auto& d = data[i];

      l.tint          = glm::vec3(d["tint"][0], d["tint"][1], d["tint"][2]);
      l.tintStrength  = d["tintStrength"];
      l.startHeight   = d["startHeight"];
      l.blendStrength = d["blendStrength"];
      l.textureScale  = d["textureScale"];
    }

    f.close();
  } else {
    warning("[Terrain::loadRegions] Could not open the file [{}]", path.string());
  }

  updateLayers();
}

void Terrain::saveLayers(std::string_view name) const {
  fspath path = fspath("res/data/layers") / layersName / name;

  std::ofstream f(path);

  if (f.is_open()) {
    json data = json::array();

    for (int i = 0; i < layersCount; i++) {
      const TerrainLayer& l = layers[i];
      json d;

      d["tint"][0] = l.tint.x;
      d["tint"][1] = l.tint.y;
      d["tint"][2] = l.tint.z;

      d["tintStrength"]  = l.tintStrength;
      d["startHeight"]   = l.startHeight;
      d["blendStrength"] = l.blendStrength;
      d["textureScale"]  = l.textureScale;

      data.push_back(d);
    }

    f << std::setw(2) << data << std::endl;
    f.close();
  } else {
    error("[Terrain::saveRegions] Could not open the file [{}]", path.string());
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
  shader.setUniform1i("u_terrainLayersCount", layersCount);
  shader.setUniform1f("u_useLighting", useLighting);
  shader.setUniform1f("u_heightMultiplier", heightMultiplier);
  shader.setUniform1f("u_maskDebugColors", showChunks);
  shader.setUniform1f("u_maskNormalmap", showChunkNormalmap);
  shader.setUniform1f("u_maskFalloffmap", useFalloffmap);
  shader.setUniform1f("u_minHeight", calcHeight(0.f));
  shader.setUniform1f("u_maxHeight", calcHeight(1.f));
  shader.setUniform2f("u_chunks", vec2(chunksPerAxis));

  ubo.bindPoint(0);
  layersTexture.bind();
  sharedMapGen.falloffTex.bind();
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

  ubo.unbindPoint(0);
  layersTexture.unbind();
  sharedMapGen.falloffTex.unbind();
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

