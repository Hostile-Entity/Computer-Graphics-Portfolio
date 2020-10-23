#version 330 core

layout(location = 0) in vec3 pos_model_space;
layout(location = 1) in vec3 norm_model_space;
layout(location = 2) in vec3 tang_model_space;
layout(location = 3) in vec2 tex_coords;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 frag_pos_model_space;
out vec3 frag_pos_world_space;
out vec3 frag_pos_t_space;
out vec3 frag_norm_world_space;
out mat3 frag_TBN;
out vec2 frag_tex_coords;

void main()
{
    vec4 pos_world_space = modelMatrix * vec4(pos_model_space, 1.0f);
    vec3 norm_world_space = normalize(normalMatrix * norm_model_space);
    vec3 tang_world_space = normalize(normalMatrix * tang_model_space);
    vec3 bitang_world_space = cross(tang_world_space, norm_world_space);

    frag_pos_model_space = pos_model_space;

    frag_pos_world_space = pos_world_space.xyz;
    
    frag_norm_world_space = norm_world_space;
    frag_TBN = mat3(tang_world_space, bitang_world_space, norm_world_space);
    frag_tex_coords = tex_coords;

    gl_Position = projectionMatrix * viewMatrix * pos_world_space;
}
