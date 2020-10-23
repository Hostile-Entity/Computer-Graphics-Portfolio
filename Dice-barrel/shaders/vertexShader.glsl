#version 330 core

layout(location = 0) in vec3 positionMS;
layout(location = 1) in vec3 normalMS;
layout(location = 2) in vec3 tangentMS;
layout(location = 3) in vec2 texCoords;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragPositionMS;
out vec3 fragPositionWS;
out vec3 fragPositionTS;
out vec3 fragNormalWS;
out mat3 fragTBN;
out vec2 fragTexCoords;

void main()
{
    vec4 positionWS = modelMatrix * vec4(positionMS, 1.0f);
    vec3 normalWS = normalize(normalMatrix * normalMS);
    vec3 tangentWS = normalize(normalMatrix * tangentMS);
    vec3 bitangentWS = cross(tangentWS, normalWS);

    fragPositionMS = positionMS;
    fragPositionWS = positionWS.xyz;

    fragNormalWS = normalWS;
    fragTBN = mat3(tangentWS, bitangentWS, normalWS);
    fragTexCoords = texCoords;

    gl_Position = projectionMatrix * viewMatrix * positionWS;
}
