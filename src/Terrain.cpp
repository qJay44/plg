#include "Terrain.hpp"

#include "MapGenerator.hpp"
#include "colormaps/jet.hpp"

Terrain::Terrain(vec3 pos) {
  build(glm::floor(vec2{pos.x, pos.z} / chunkSize));
}

void Terrain::update(vec3 pos, bool force) {
  ivec2 coord00 = glm::floor(vec2{pos.x, pos.z} / chunkSize);

  if (autoChunkSize)
    chunkSize = sharedMapGen.size.x * 0.5f;

  if ((chunk00Coord != coord00 && attachCam) || force)
    build(coord00);
}

void Terrain::draw(const Camera* camera, const Shader& shader, bool forceNoWireframe) const {
  shader.setUniform1i("u_div", sharedMapGen.tescDiv);
  shader.setUniform1f("u_heightMultiplier", sharedMapGen.heightMultiplier);
  shader.setUniform1i("u_useDebugColors", useDebugColors);

  constexpr float _jetLenf = colormaps::jet.size();
  float colormapStep = _jetLenf / chunks.size();

  for (size_t i = 0; i < chunks.size(); i++) {
    const TerrainChunk& tc = chunks[i];

    shader.setUniformTexture(tc.mg.terrainTex);
    shader.setUniform3f("u_chunkDebugColor", colormaps::jet[static_cast<size_t>(i * colormapStep)]);

    tc.mg.terrainTex.bind();
    tc.draw(camera, shader, forceNoWireframe);
    tc.mg.terrainTex.unbind();
  }
}

void Terrain::build(ivec2 coord00) {
  chunksTotal = chunksPerAxis * chunksPerAxis;
  chunk00Coord = coord00;

  if (chunks.size() != chunksTotal)
    chunks.resize(chunksTotal);

  const int distFromMiddle = chunksPerAxis / 2;

  for (int i = 0; i < chunksPerAxis; i++) {
    for (int j = 0; j < chunksPerAxis; j++) {
      TerrainChunk& tc = chunks[j + i * chunksPerAxis];

      ivec2 coord = ivec2{j, i} - distFromMiddle; // offset to build around camera (centered)
      vec2 chunkCoord = vec2(chunk00Coord + coord);
      vec2 chunkPos = chunkCoord * chunkSize;
      vec2 chunkCenterPos = chunkPos + chunkSize * 0.5f; // center of a chunk

      MapGenerator mg(sharedMapGen);
      mg.offset = sharedMapGen.offset;
      mg.offset += chunkPos * 2.f; // Chunk position offset
      mg.offset.y *= -1.f;

      tc.clear();
      tc = TerrainChunk(chunkResolution, {chunkCenterPos.x, 0.f, chunkCenterPos.y}, chunkSize, mg);
    }
  }
}

