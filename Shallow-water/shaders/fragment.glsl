#version 330 core

in float diff;
in vec2 UV;
in float spec;

uniform sampler2D textureID;
uniform int material;

out vec4 color;

void main()
{
	
	float ambient = 0.3;
	
	float diffuse = diff;
	
	float specular = spec;
	
	
	float alpha;
	if (material == 1) {
		alpha = 0.5;
		color = vec4((ambient + diffuse + specular) * texture(textureID, UV).rgb, alpha);
	} else if (material == 2) {
		alpha = 1;
		color = vec4(texture(textureID, UV).rgb, alpha);
	}else {
		alpha = 1;
		color = vec4((ambient + diffuse + specular) * texture(textureID, UV).rgb, alpha);
	}
}