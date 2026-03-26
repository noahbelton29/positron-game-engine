#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aUV;

out vec3 vColor;
out vec2 vUV;

uniform mat4  uMVP;
uniform float uTiling;

void main() {
    vColor      = aColor;
    vUV         = aUV * uTiling;
    gl_Position = uMVP * vec4(aPos, 1.0);
}
