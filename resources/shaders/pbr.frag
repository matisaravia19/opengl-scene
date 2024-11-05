#version 460 core

#define PI 3.1415926538

in vec3 fWorldPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 oFragColor;

uniform vec3 cameraPosition;

layout (binding = 0) uniform sampler2D albedo;

const vec3 lightPos = vec3(1.0, 2.0, 2.0);
const vec3 lightColor = vec3(10.0, 10.0, 10.0);

float saturate(float x) {
    return clamp(x, 0.0, 1.0);
}

float positiveDot(vec3 a, vec3 b) {
    return max(dot(a, b), 0.0);
}

vec3 fresnelSchlick(vec3 halfwayDir, vec3 viewDir, vec3 albedo, float metallic) {
    vec3 f0 = mix(vec3(0.04), albedo, metallic);
    return f0 + (1.0 - f0) * pow(saturate(1.0 - positiveDot(halfwayDir, viewDir)), 5.0);
}

float distributionGGX(vec3 normal, vec3 halfwayDir, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;

    float nDotH = positiveDot(normal, halfwayDir);
    float nDotH2 = nDotH * nDotH;

    float numerator = a2;
    float denominator = (nDotH * (a2 - 1.0) + 1.0);
    denominator = PI * denominator * denominator;

    return numerator / denominator;
}

float geometrySchlickGGX(vec3 normal, vec3 direction, float roughness) {
    float nDotD = positiveDot(normal, direction);

    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float numerator = nDotD;
    float denominator = nDotD * (1.0 - k) + k;

    return numerator / denominator;
}

float geometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) {
    float ggx2 = geometrySchlickGGX(normal, viewDir, roughness);
    float ggx1 = geometrySchlickGGX(normal, lightDir, roughness);

    return ggx1 * ggx2;
}

vec3 pbr(vec3 albedo, float metallic, float roughness) {
    vec3 fragToLight = lightPos - fWorldPosition;
    vec3 fragToCamera = cameraPosition - fWorldPosition;

    vec3 lightDir = normalize(fragToLight);
    vec3 viewDir = normalize(fragToCamera);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float distanceToLight = length(fragToLight);
    float attenuation = 1.0 / (distanceToLight * distanceToLight);
    vec3 radiance = lightColor * attenuation;

    vec3 f = fresnelSchlick(halfwayDir, viewDir, albedo, metallic);
    float ndf = distributionGGX(fNormal, halfwayDir, roughness);
    float g = geometrySmith(fNormal, viewDir, lightDir, roughness);

    vec3 kS = f;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    vec3 diffuse = kD * albedo / PI;

    vec3 numerator = ndf * g * f;
    float denominator = 4.0 * positiveDot(fNormal, viewDir) * positiveDot(fNormal, lightDir) + 0.0001;
    vec3 specular = numerator / denominator;

    return (diffuse + specular) * radiance * positiveDot(fNormal, lightDir);
}

void main() {
    vec3 color = pbr(texture(albedo, fTexCoords).rgb, 0.0, 0.1);

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    oFragColor = vec4(color, 1.0);
}