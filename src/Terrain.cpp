#include "Terrain.hpp"

#include "MapGenerator.hpp"

Terrain::Terrain(vec3 pos) {
  build(glm::floor(vec2{pos.x, pos.z} / chunkSize));
}

void Terrain::update(vec3 pos, bool force) {
  ivec2 coord00 = glm::floor(vec2{pos.x, pos.z} / chunkSize);

  if (autoChunkSize)
    chunkSize = sharedMapGen.size.x * 0.5f;

  if ((chunk00Coord != coord00) || force)
    build(coord00);
}

void Terrain::draw(const Camera* camera, const Shader& shader, bool forceNoWireframe) const {
  static constexpr vec3 chunkDebugColors[] = {
    {0.200f, 0.659f, 0.780f}, // #33a8c7
    {0.322f, 0.890f, 0.882f}, // #52e3e1
    {0.627f, 0.894f, 0.149f}, // #a0e426
    {0.992f, 0.945f, 0.282f}, // #fdf148
    {1.000f, 0.671f, 0.000f}, // #ffab00
    {0.969f, 0.475f, 0.463f}, // #f77976
    {0.941f, 0.314f, 0.682f}, // #f050ae
    {0.847f, 0.514f, 1.000f}, // #d883ff
    {0.576f, 0.212f, 0.992f}  // #9336fd
  };

  shader.setUniform1i("u_div", sharedMapGen.tescDiv);
  shader.setUniform1f("u_heightMultiplier", sharedMapGen.heightMultiplier);
  shader.setUniform1i("u_useDebugColors", useDebugColors);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      const TerrainChunk& tc = chunks[i][j];

      shader.setUniformTexture(tc.mg.terrainTex);
      shader.setUniform3f("u_chunkDebugColor", chunkDebugColors[j + i * 3]);

      tc.mg.terrainTex.bind();
      tc.draw(camera, shader, forceNoWireframe);
      tc.mg.terrainTex.unbind();
    }
  }
}

void Terrain::build(ivec2 coord00) {
  if (attachCam)
    chunk00Coord = coord00;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      TerrainChunk& tc = chunks[i][j];

      ivec2 coord = ivec2{j, i} - 1;
      vec2 chunkCoord = vec2(chunk00Coord + coord);
      vec2 chunkPos = chunkCoord * chunkSize;

      MapGenerator mg(sharedMapGen);
      mg.offset = sharedMapGen.offset + chunkPos + chunkSize * vec2(coord);
      mg.offset.y *= -1.f;

      tc.clear();
      tc = TerrainChunk(chunkResolution, {chunkPos.x, 0.f, chunkPos.y}, chunkSize, mg);
    }
  }
}

