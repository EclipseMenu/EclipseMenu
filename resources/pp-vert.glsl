#version 120

attribute vec2 aPosition;
attribute vec2 aTexCoords;

varying vec2 TexCoords;

void main() {
    gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
    TexCoords = aTexCoords;
}