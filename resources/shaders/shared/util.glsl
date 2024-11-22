#define PI 3.1415926538

float saturate(float x) {
    return clamp(x, 0.0, 1.0);
}

float positiveDot(vec3 a, vec3 b) {
    return max(dot(a, b), 0.0);
}