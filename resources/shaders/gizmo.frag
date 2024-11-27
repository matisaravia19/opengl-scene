#version 460 core

out vec4 oFragColor;

uniform vec4 color;

void main() {
    oFragColor = color;
}