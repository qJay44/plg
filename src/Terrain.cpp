#include "Terrain.hpp"

#include "MapGenerator.hpp"
#include "colormaps/jet.hpp"

Terrain::Terrain(vec3 pos) {
  update(pos, true);
}

void Terrain::update(vec3 pos, bool force) {
  vec2 posOnChunk{pos.x, pos.z};
  ivec2 coord00 = glm::floor(posOnChunk / chunkSize);

  if (autoChunkSize)
    chunkSize = sharedMapGen.size.x * 0.5f;

  if ((chunk00Coord != coord00 && attachCam) || force) {
    sharedMapGen.offset = coord00 * sharedMapGen.size.x;
    sharedMapGen.offset /= chunksPerAxis;
    sharedMapGen.offset.y *= -1.f;
    sharedMapGen.gen();
    build(coord00);
  }
}

void Terrain::draw(const Camera* camera, const Shader& shader, bool forceNoWireframe) const {
  shader.setUniform1i("u_div", sharedMapGen.tescDiv);
  shader.setUniform1f("u_heightMultiplier", sharedMapGen.heightMultiplier);
  shader.setUniform1i("u_useDebugColors", useDebugColors);

  shader.setUniformTexture(sharedMapGen.terrainTex);
  shader.setUniform2f("u_chunks", vec2(chunksPerAxis));
  sharedMapGen.terrainTex.bind();

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

  sharedMapGen.terrainTex.unbind();
}

void Terrain::build(ivec2 coord00) {
  chunksTotal = chunksPerAxis * chunksPerAxis;
  chunk00Coord = coord00;

  if (chunks.size() != chunksTotal)
    chunks.resize(chunksTotal);

  const int distFromMiddle = chunksPerAxis / 2; // offset to build around camera (centered)

  for (int i = 0; i < chunksPerAxis; i++) {
    for (int j = 0; j < chunksPerAxis; j++) {
      TerrainChunk& tc = chunks[j + i * chunksPerAxis];

      ivec2 coord = ivec2{j, chunksPerAxis - 1 - i};
      vec2 chunkCoord = vec2(chunk00Coord + coord - distFromMiddle);
      vec2 chunkPos = chunkCoord * chunkSize;
      vec2 chunkCenterPos = chunkPos + chunkSize * 0.5f; // center of a chunk

      tc.clear();
      tc = TerrainChunk(chunkResolution, {chunkCenterPos.x, 0.f, chunkCenterPos.y}, chunkSize);
    }
  }
}

