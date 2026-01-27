#version 460 core

in DATA {
  vec4 vertPos;
  vec2 texCoord;
  vec2 chunkTexCoord;
  vec3 chunkNormal;
} dataIn;

out vec4 FragColor;

uniform sampler2D u_terrainTex;
uniform vec3 u_chunkDebugColor;
uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_camForward;
uniform float u_sunFocus;
uniform float u_sunIntensity;
uniform float u_maskDebugColors;
uniform float u_maskNormalmap;

vec3 directionalLight(vec3 col) {
  vec3 lightDir = normalize(u_lightPos - dataIn.vertPos.xyz);
  vec3 viewDir = normalize(u_camPos - dataIn.vertPos.xyz);
  vec3 reflectDir = reflect(-lightDir, dataIn.chunkNormal);

  float diffuse = max(dot(dataIn.chunkNormal, lightDir), 0.f);
  float ambient = 0.2f;
  float specularLight = 0.5f;
  float specAmount = pow(max(dot(viewDir, reflectDir), 0.f), 8);
  float specular = specAmount * specularLight;

  return col * (diffuse + ambient + specular);
}

void main() {
  vec3 col = texture(u_terrainTex, dataIn.chunkTexCoord).rgb;
  col = col * (1.f - u_maskDebugColors) + u_chunkDebugColor * u_maskDebugColors;
  col = col * (1.f - u_maskNormalmap) + (dataIn.chunkNormal * 0.5f + 0.5f) * u_maskNormalmap;
  col = directionalLight(col);

  FragColor = vec4(col, 1.f);
}

