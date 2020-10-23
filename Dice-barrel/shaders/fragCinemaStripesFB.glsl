#version 330 core

in vec3 fragPositionMS;
in vec2 fragTexCoords;

uniform sampler2D diffuseTexture;

out vec4 fragColor;

void main()
{
    float stripeSize = 0.07f;
    if (abs(fragPositionMS.y) > (1.0f - 2.0f * stripeSize))
    {
        fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        fragColor = texture(diffuseTexture, fragTexCoords); 
    }
}
