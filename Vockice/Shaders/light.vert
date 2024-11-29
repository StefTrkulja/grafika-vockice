#version 330 core

layout (location = 0) in vec2 aPos;   // Pozicije verteksa
layout (location = 1) in vec2 aTexCoord; // Teksturne koordinate

out vec2 TexCoords; // Prosle?ujemo teksturne koordinate u fragment šejder

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0); // Postavljamo poziciju verteksa u prostor ekrana
    TexCoords = aTexCoord;             // Prosle?ujemo teksturne koordinate
}
