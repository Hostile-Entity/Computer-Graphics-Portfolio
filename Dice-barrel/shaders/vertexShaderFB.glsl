#version 330 core

layout(location = 0) in vec3 positionMS;
layout(location = 3) in vec2 texCoords;

out vec3 fragPositionMS;
out vec2 fragTexCoords;

void main()
{
    fragPositionMS = positionMS;
    fragTexCoords = vec2(texCoords.x, 1.0f - texCoords.y);
    gl_Position = vec4(positionMS, 1.0f);
}
