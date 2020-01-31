#version 330

in float passBasicLight;
in vec3 passTexCoord;
in float passAO;

out vec4 outColour;

uniform sampler2DArray textureArray;

void main() {
    outColour = passAO * passBasicLight * texture(textureArray, passTexCoord);
}