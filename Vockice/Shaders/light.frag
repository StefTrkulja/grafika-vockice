#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture1;
uniform float intensity; // Dodata uniform promenljiva

void main() {
    vec4 texColor = texture(texture1, TexCoords);
    if (texColor.a < 0.1) // Ignorisi gotovo providne piksele
        discard;
    FragColor = texColor * intensity; // Modifikuj osvetljenje
}
