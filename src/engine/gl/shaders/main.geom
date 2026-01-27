#version 460 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in DATA {
  vec4 vertPos;
  vec2 texCoord;
  vec2 chunkTexCoord;
  vec3 chunkNormal;
} dataIn[];

out DATA {
  vec4 vertPos;
  vec2 texCoord;
  vec2 chunkTexCoord;
  vec3 chunkNormal;
} dataOut;

uniform mat4 u_camPV;

void main() {
  for (int i = 0; i < 3; i++) {
    dataOut.vertPos       = dataIn[i].vertPos;
    dataOut.texCoord      = dataIn[i].texCoord;
    dataOut.chunkTexCoord = dataIn[i].chunkTexCoord;
    dataOut.chunkNormal   = dataIn[i].chunkNormal;

    gl_Position = u_camPV * dataIn[i].vertPos;
    EmitVertex();

    dataOut.vertPos       = dataIn[i].vertPos;
    dataOut.texCoord      = dataIn[i].texCoord;
    dataOut.chunkTexCoord = dataIn[i].chunkTexCoord;
    dataOut.chunkNormal   = dataIn[i].chunkNormal;

    gl_Position = u_camPV * (dataIn[i].vertPos + vec4(dataIn[i].chunkNormal, 0.f) * 2.f);
    EmitVertex();

    EndPrimitive();
  }
}

