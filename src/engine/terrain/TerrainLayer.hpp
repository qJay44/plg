#pragma once

struct TerrainLayer {
  alignas(16) vec3 tint;
  float tintStrength;
  float startHeight;
  float blendStrength;
  float textureScale;
};

