#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 uLightSpaceMVP;

void main() {
    gl_Position = uLightSpaceMVP * vec4(aPos, 1.0);
}
