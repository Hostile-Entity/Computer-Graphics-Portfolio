#version 330 core

in vec3 frag_pos_model_space;
in vec3 frag_pos_world_space;
in vec3 frag_pos_t_space;
in vec3 frag_norm_world_space;
in mat3 frag_TBN;
in vec2 frag_tex_coords;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D heightTexture;
uniform sampler2D shadowTexture;

uniform vec3 cameraPosWS;
uniform vec3 ligth_pos_world_space;
uniform mat4 shadowSpaceMatrix;

out vec4 frag_color;

// Shadow Map
float readDepthMap(vec2 texCoords, vec2 texelSize)
{
    vec2 pixelPos = texCoords / texelSize + 0.5f;
    vec2 fractPart = fract(pixelPos);
    vec2 startTexel = (pixelPos - fractPart) * texelSize;

    float texelBL = texture(shadowTexture, startTexel).r;
    float texelBR = texture(shadowTexture, startTexel + vec2(texelSize.x, 0.0f)).r;
    float texelTL = texture(shadowTexture, startTexel + vec2(0.0f, texelSize.y)).r;
    float texelTR = texture(shadowTexture, startTexel + texelSize).r;

    float maxLeft = mix(texelBL, texelTL, fractPart.y);
    float maxRight = mix(texelBR, texelTR, fractPart.y);

    return mix(maxLeft, maxRight, fractPart.x);
}

float enlighten(vec3 normalWS, vec3 lightPositionWS)
{
    // Ambient part
    float ambient = 0.1f;
    
    // Difuse part
    vec3 lightDirUnnormWS = lightPositionWS - frag_pos_world_space;
    vec3 lightDirWS = normalize(lightDirUnnormWS);
    float diffuse = max(0.0f, dot(normalWS, lightDirWS));

    // Specular part
    vec3 viewDirWS = normalize(cameraPosWS - frag_pos_world_space);
    vec3 reflectDirWS = reflect(-lightDirWS, normalWS);
    float specular = 0.75f * pow(max(0.0f, dot(viewDirWS, reflectDirWS)), 32);

    // Attenuation part
    float constAttenuation = 0.1f;
    float linearAttenuation = 0.3f;
    float quadraticAttenumation = 0.5f;
    float lightDistance = length(lightDirUnnormWS);
    float lightIntensity = 40.0f;
    float attenuation = lightIntensity / (constAttenuation + linearAttenuation * lightDistance + quadraticAttenumation * lightDistance * lightDistance);
    
    // Shade part
    vec4 fragPositionLS = shadowSpaceMatrix * vec4(frag_pos_world_space, 1.0f);
    vec3 projCoords = fragPositionLS.xyz / fragPositionLS.w * 0.5f + 0.5f;

    vec2 texelSize = 1.0f / textureSize(shadowTexture, 0);

    float depthCurrent = projCoords.z;

    float maxBias = texelSize.x / 2.0f;
    float bias = max(maxBias, 0.1f * maxBias * (1.0f - dot(normalWS, lightDirWS)));

    float shadow = 0.0f;
    float numSamples = 16.0f;
    float maxSample = (numSamples - 1.0f) / 2.0f;
    for (float x = -maxSample; x <= maxSample; x += 1.0f)
    {
        for (float y = -maxSample; y <= maxSample; y += 1.0f)
        {
            float depthPCF = readDepthMap(projCoords.xy + vec2(x, y) * texelSize, texelSize);
            shadow = shadow + ( depthCurrent - bias > depthPCF ? 1.0f : 0.0f );
        }
    }
    shadow = shadow / (numSamples * numSamples);
    
    return attenuation * (ambient + (1.0f - shadow) * (diffuse + specular));    
}

vec3 getBumpedNormal(vec2 texCoords)
{
    vec3 bumpedNormal = texture(normalTexture, texCoords).xyz;
    bumpedNormal = 2.0f * bumpedNormal - 1.0f;
    bumpedNormal = normalize(frag_TBN * bumpedNormal);
    return bumpedNormal;
}

vec2 getParallaxTexCoords()
{
    vec3 viewDirTS = normalize(transpose(frag_TBN) * (cameraPosWS - frag_pos_world_space));
    
    float minNumLayers = 10.0f;
    float maxNumLayers = 20.0f;
    float numLayers = mix(maxNumLayers, minNumLayers, abs(viewDirTS.z));
    float layerDepth = 1.0f / numLayers;

    float heightScale = 0.05f;
    vec2 parallaxOffset = heightScale * viewDirTS.xy / viewDirTS.z;
    vec2 deltaTexCoords = parallaxOffset / numLayers;

    vec2 curTexCoords = frag_tex_coords;
    float curDepthValue = texture(heightTexture, curTexCoords).r;
    float curLayerDepth = 0.0f;
    while (curLayerDepth < curDepthValue)
    {
        curTexCoords -= deltaTexCoords;
        curDepthValue = texture(heightTexture, curTexCoords).r;
        curLayerDepth += layerDepth;
    }

    vec2 prevTexCoords = curTexCoords + deltaTexCoords;
    float depthBeforeCollision = texture(heightTexture, curTexCoords).r - curLayerDepth + layerDepth;
    float depthAfterCollision = curDepthValue - curLayerDepth;

    float weight = depthAfterCollision / (depthAfterCollision - depthBeforeCollision);
    vec2 texCoords = mix(curTexCoords, prevTexCoords, weight);

    if (texCoords.x < 0.0f || texCoords.x > 1.0f || texCoords.y < 0.0f || texCoords.y > 1.0f)
        discard;
    
    return texCoords;
}

void main()
{
    vec2 texCoords = getParallaxTexCoords();
    vec3 bumpedNormal = getBumpedNormal(texCoords);
    frag_color = texture(diffuseTexture, texCoords) * enlighten(bumpedNormal, ligth_pos_world_space);
}
