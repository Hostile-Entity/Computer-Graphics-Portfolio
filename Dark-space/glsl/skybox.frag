#version 330 core

in vec3 frag_pos_model_space;

uniform samplerCube diffuseTexture;

out vec4 frag_color;

void main()
{
    frag_color = texture(diffuseTexture, frag_pos_model_space);
}
