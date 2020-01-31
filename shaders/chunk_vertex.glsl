#version 330

layout (location = 0) in vec3 inVertexCoord;
layout (location = 1) in vec3 inTextureCoord;
layout (location = 2) in float inBasicLight;
layout (location = 3) in float inAO;

uniform mat4 projectionViewMatrix;

out vec3 passTexCoord;
out float passBasicLight;
out float passAO;

void main() {
    gl_Position = projectionViewMatrix * vec4(inVertexCoord, 1.0);
    
    passTexCoord = inTextureCoord;
    passBasicLight = inBasicLight;
    passAO = inAO;
}