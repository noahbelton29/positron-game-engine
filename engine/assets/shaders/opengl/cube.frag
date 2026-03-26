#version 460 core

#define MAX_LIGHTS      16
#define MAX_SHADOW_MAPS 4
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT       1
#define LIGHT_SPOT        2

struct Light {
    int   type;
    vec3  position;
    vec3  direction;
    vec3  color;
    float intensity;
    float range;
    float attenLinear;
    float attenQuad;
    float innerAngleCos;
    float outerAngleCos;
    bool  castShadows;
    int   shadowMapIndex;
    mat4  lightSpaceMatrix;
};

in vec3 vColor;
in vec2 vUV;
in vec3 vWorldPos;
in vec3 vNormal;

out vec4 fragColor;

uniform sampler2D uAlbedo;
uniform int       uHasTexture;
uniform vec4      uTint;
uniform vec3      uCameraPos;

uniform vec3  uAmbientColor;
uniform float uAmbientIntensity;
uniform int   uLightCount;
uniform Light uLights[MAX_LIGHTS];

uniform sampler2D uShadowMaps[MAX_SHADOW_MAPS];

float shadowFactor(int smIndex, mat4 lightSpaceMat) {
    vec4  fragLS    = lightSpaceMat * vec4(vWorldPos, 1.0);
    vec3  proj      = fragLS.xyz / fragLS.w * 0.5 + 0.5;

    if (proj.z > 1.0)
        return 0.0;

    vec3  toLight   = normalize(-vec3(lightSpaceMat[0][2], lightSpaceMat[1][2], lightSpaceMat[2][2]));
    float cosTheta  = clamp(dot(normalize(vNormal), toLight), 0.0, 1.0);
    float bias      = max(0.002, 0.008 * (1.0 - cosTheta));

    float shadow    = 0.0;
    vec2  texelSize = 1.0 / textureSize(uShadowMaps[smIndex], 0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(uShadowMaps[smIndex], proj.xy + vec2(x, y) * texelSize).r;
            shadow += (proj.z - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow * 0.75;
}

float attenuation(Light l, float dist) {
    if (l.type == LIGHT_DIRECTIONAL) return 1.0;
    float a = 1.0 / (1.0 + l.attenLinear * dist + l.attenQuad * dist * dist);
    return clamp(a * (1.0 - dist / l.range), 0.0, 1.0);
}

vec3 calcLight(Light l, vec3 normal, vec3 viewDir, vec3 albedo) {
    vec3  toLight  = l.type == LIGHT_DIRECTIONAL ? normalize(-l.direction) : normalize(l.position - vWorldPos);
    float dist     = l.type == LIGHT_DIRECTIONAL ? 0.0 : length(l.position - vWorldPos);
    float atten    = attenuation(l, dist);

    float diff     = max(dot(normal, toLight), 0.0);
    vec3  halfVec  = normalize(toLight + viewDir);
    float spec     = pow(max(dot(normal, halfVec), 0.0), 32.0);

    float spotMask = 1.0;
    if (l.type == LIGHT_SPOT) {
        float theta  = dot(toLight, normalize(-l.direction));
        float eps    = l.innerAngleCos - l.outerAngleCos;
        spotMask     = clamp((theta - l.outerAngleCos) / eps, 0.0, 1.0);
    }

    float shadow = 0.0;
    if (l.castShadows && l.shadowMapIndex >= 0)
        shadow = shadowFactor(l.shadowMapIndex, l.lightSpaceMatrix);

    vec3 diffuse  = diff * l.color * l.intensity * albedo;
    vec3 specular = spec * l.color * l.intensity * 0.3;
    return (diffuse + specular) * atten * spotMask * (1.0 - shadow);
}

void main() {
    vec4 base = uHasTexture == 1
        ? texture(uAlbedo, vUV)
        : vec4(vColor, 1.0);
    base *= uTint;

    vec3 albedo  = base.rgb;
    vec3 normal  = normalize(vNormal);
    vec3 viewDir = normalize(uCameraPos - vWorldPos);

    vec3 ambient = uAmbientColor * uAmbientIntensity * albedo;
    vec3 result  = ambient;

    for (int i = 0; i < min(uLightCount, MAX_LIGHTS); i++)
        result += calcLight(uLights[i], normal, viewDir, albedo);

    fragColor = vec4(result, base.a);
}
