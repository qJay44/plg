#version 460 core

#define TERRAIN_LAYERS 8 // NOTE: Must match on the CPU side
#define EPSILON 1e-4f

in DATA {
  vec4 vertPos;
  vec2 texCoord;
  vec2 chunkTexCoord;
  vec3 chunkNormal;
} dataIn;

out vec4 FragColor;

struct TerrainLayer {
  vec3 tint;
  float tintStrength;
  float startHeight;
  float blendStrength;
  float textureScale;
};

layout (std140, binding = 0) uniform TerrainLayersBlock {
  TerrainLayer u_terrainLayers[TERRAIN_LAYERS];
};

layout(binding = 0) uniform sampler2DArray u_terrainLayersTex;

uniform vec3 u_chunkDebugColor;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_camPos;
uniform vec3 u_camForward;
uniform float u_useLighting;
uniform float u_sunFocus;
uniform float u_sunIntensity;
uniform float u_maskDebugColors;
uniform float u_maskNormalmap;
uniform float u_minHeight;
uniform float u_maxHeight;
uniform int u_terrainLayersCount;

vec3 directionalLight() {
  vec3 lightDir = normalize(u_lightPos - dataIn.vertPos.xyz);
  vec3 viewDir = normalize(u_camPos - dataIn.vertPos.xyz);
  vec3 reflectDir = reflect(-lightDir, dataIn.chunkNormal);

  float diffuse = max(dot(dataIn.chunkNormal, lightDir), 0.f);
  float ambient = 0.2f;
  float specularLight = 0.5f;
  float specAmount = pow(max(dot(viewDir, reflectDir), 0.f), 8);
  float specular = specAmount * specularLight;

  return u_lightColor * (diffuse + ambient + specular);
}

float inverseLerp(float a, float b, float n) {
  return clamp((n - a) / (b - a), 0.f, 1.f);
}

vec3 triplanarSample(int i) {
  vec3 scaledVertPos = dataIn.vertPos.xyz * u_terrainLayers[i].textureScale;
  vec3 blendAxis = abs(dataIn.chunkNormal);
  vec3 xProj = texture(u_terrainLayersTex, vec3(scaledVertPos.yz, i)).rgb * blendAxis.x;
  vec3 yProj = texture(u_terrainLayersTex, vec3(scaledVertPos.xz, i)).rgb * blendAxis.y;
  vec3 zProj = texture(u_terrainLayersTex, vec3(scaledVertPos.xy, i)).rgb * blendAxis.z;

  return xProj + yProj + zProj;
}

void main() {
  vec3 col = vec3(0.f);
  float height = smoothstep(u_minHeight, u_maxHeight, dataIn.vertPos.y);

  for (int i = 0; i < u_terrainLayersCount; i++) {
    TerrainLayer terrainLayer = u_terrainLayers[i];
    float b = terrainLayer.blendStrength;
    float drawStrength = smoothstep(-b * 0.5f - EPSILON, b * 0.5f, height - terrainLayer.startHeight);

    vec3 baseColor = terrainLayer.tint * terrainLayer.tintStrength;
    vec3 textureColor = triplanarSample(i) * (1.f - terrainLayer.tintStrength);

    col = col * (1.f - drawStrength) + (baseColor + textureColor) * drawStrength;
  }

  col = col * (1.f - u_maskDebugColors) + u_chunkDebugColor * u_maskDebugColors;
  col = col * (1.f - u_maskNormalmap) + (dataIn.chunkNormal * 0.5f + 0.5f) * u_maskNormalmap;

  col = col * directionalLight() * u_useLighting + col * (1.f - u_useLighting);

  FragColor = vec4(col, 1.f);
}

