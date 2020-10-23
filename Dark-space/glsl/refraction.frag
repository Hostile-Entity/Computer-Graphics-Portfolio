#version 330 core

in vec3 frag_pos_model_space;
in vec3 frag_pos_world_space;
in vec3 frag_norm_world_space;

uniform samplerCube diffuseTexture;
uniform vec3 cameraPosWS;

out vec4 frag_color;

void main()
{
    vec3 viewDirWS = normalize(cameraPosWS - frag_pos_world_space);

    frag_color = texture(diffuseTexture, refract(-viewDirWS, frag_norm_world_space, 2.0f / 3.14f));
}
