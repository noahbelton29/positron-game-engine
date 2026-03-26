#version 460 core

in vec3 vColor;
in vec2 vUV;

out vec4 fragColor;

uniform sampler2D uAlbedo;
uniform int       uHasTexture;
uniform vec4      uTint;

void main() {
    vec4 base = uHasTexture == 1
        ? texture(uAlbedo, vUV)
        : vec4(vColor, 1.0);

    fragColor = base * uTint;
}
