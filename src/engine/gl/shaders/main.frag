#version 460 core

#define TERRAIN_REGIONS 8 // NOTE: Must match on the CPU side

in DATA {
  vec4 vertPos;
  vec2 texCoord;
  vec2 chunkTexCoord;
  vec3 chunkNormal;
} dataIn;

out vec4 FragColor;

uniform sampler2D u_noiseTex;
uniform vec3 u_chunkDebugColor;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_camPos;
uniform vec3 u_camForward;
uniform vec3 u_terrainColors[TERRAIN_REGIONS];
uniform float u_sunFocus;
uniform float u_sunIntensity;
uniform float u_maskDebugColors;
uniform float u_maskNormalmap;
uniform float u_minHeight;
uniform float u_maxHeight;
uniform float u_terrainHeights[TERRAIN_REGIONS];
uniform int u_terrainRegions;

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
  return (n - a) / (b - a);
}

void main() {
  vec3 col = vec3(texture(u_noiseTex, dataIn.chunkTexCoord).r);
  float height = inverseLerp(u_minHeight, u_maxHeight, dataIn.vertPos.y);

  for (int i = 0; i < u_terrainRegions; i++) {
    float drawStrength = max(sign(height - u_terrainHeights[i]), 0.f);
    col = col * (1.f - drawStrength) + u_terrainColors[i] * drawStrength;
  }

  col = col * (1.f - u_maskDebugColors) + u_chunkDebugColor * u_maskDebugColors;
  col = col * (1.f - u_maskNormalmap) + (dataIn.chunkNormal * 0.5f + 0.5f) * u_maskNormalmap;

  col *= directionalLight();

  FragColor = vec4(col, 1.f);
}

