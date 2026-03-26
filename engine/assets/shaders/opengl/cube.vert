#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aUV;

out vec3 vColor;
out vec2 vUV;
out vec3 vWorldPos;
out vec3 vNormal;

uniform mat4  uMVP;
uniform mat4  uModel;
uniform mat3  uNormalMatrix;
uniform float uTiling;

void main() {
    vColor      = aColor;
    vUV         = aUV * uTiling;
    vWorldPos   = vec3(uModel * vec4(aPos, 1.0));
    vNormal     = normalize(uNormalMatrix * aNormal);
    gl_Position = uMVP * vec4(aPos, 1.0);
}
