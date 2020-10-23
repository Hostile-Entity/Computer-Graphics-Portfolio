#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 vertUV;
layout(location = 2) in vec3 normal;

uniform mat4 MVP;
uniform mat4 ModelID;
uniform mat4 ViewID;

out float diff;
out vec2 UV;
out float spec;

void main(void)
{
	
	vec3 light_position = vec3(4,3,4);
	
	vec3 object_position = (ModelID * vec4(position,1)).xyz;
	vec3 normal_camspace = normalize((ViewID * ModelID * vec4(normal,0)).xyz);
	vec3 eye_dir_camspace = -(ViewID * vec4(object_position,1)).xyz;
	vec3 light_dir_camspace = normalize((ViewID * vec4(light_position,1)).xyz + eye_dir_camspace);
	
	diff = clamp(dot(normal_camspace, light_dir_camspace), 0, 1);
	
	spec = pow(clamp(dot(normalize(eye_dir_camspace), reflect(-light_dir_camspace, normal_camspace)), 0, 1),16);
	
	gl_Position = MVP * vec4(position, 1.0f);
	UV = vertUV;
}
