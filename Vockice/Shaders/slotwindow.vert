#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex;
out vec2 chTex;

uniform float xOffset; // Dodali smo uniform za pomeranje po X osi
uniform float yOffset; // Pomeranje po Y osi

void main()
{
    gl_Position = vec4(inPos.x + xOffset, inPos.y + yOffset, 0.0, 1.0);
    chTex = inTex;
}
