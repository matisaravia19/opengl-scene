#version 330 core
in vec3 texCoords;

uniform vec3 ambientLight;
uniform samplerCube skyboxDay;
uniform samplerCube skyboxNight;
uniform samplerCube skyboxSunset;
uniform float timeOfDay;    // Range 0.0 to 24.0
uniform vec3 sunPosition;   // Calculated based on time
uniform float starBrightness;

out vec4 FragColor;

vec4 getSunGlow(vec3 direction, vec3 sunPos) {
    float sunDot = max(dot(direction, normalize(sunPos)), 0.0);
    vec4 sunColor = vec4(1.0, 0.9, 0.7, 1.0);
    return sunColor * pow(sunDot, 256.0);
}

void main() {
    // Calculate blend factors based on time of day
    float dayTime = smoothstep(6.0, 8.0, timeOfDay) - smoothstep(16.0, 18.0, timeOfDay);
    float nightTime = 1.0 + smoothstep(18.0, 20.0, timeOfDay) - smoothstep(4.0, 6.0, timeOfDay);
    float sunsetTime = 1.0 - dayTime - nightTime;

    // Sample from each cubemap
    vec4 dayColor = texture(skyboxDay, texCoords);
    vec4 nightColor = texture(skyboxNight, texCoords);
    vec4 sunsetColor = texture(skyboxSunset, texCoords);

    // Blend between the textures
    vec4 finalColor = (dayColor * dayTime +
    nightColor * nightTime +
    sunsetColor * sunsetTime);

    // Add sun glow
    finalColor += getSunGlow(normalize(texCoords), sunPosition) * (dayTime + sunsetTime * 0.5);

    FragColor = finalColor;
}