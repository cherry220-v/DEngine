#version 330 core

#define MAX_LIGHTS 8

in vec3 FragPos;
in vec3 FragNormal;
in vec2 TexCoord;
in vec4 FragPosLightSpace[MAX_LIGHTS];

out vec4 FragColor;

uniform sampler2D texture1;
uniform bool useTexture;

uniform vec3 viewPos;
uniform vec4 objectColor;

uniform float shininess;
uniform float specularStrength;

uniform int lightCount;

uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightDirections[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightIntensities[MAX_LIGHTS];
uniform int lightTypes[MAX_LIGHTS];

uniform sampler2D shadowMaps[MAX_LIGHTS];
uniform int hasShadowMap[MAX_LIGHTS];

float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (
        projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0
    )
    {
        return 1.0;
    }

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = 0.0005;

    return currentDepth - bias > closestDepth ? 0.3 : 1.0;
}

void main()
{
    vec3 normal = normalize(FragNormal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 lighting = vec3(0.0);

    for (int i = 0; i < lightCount; i++)
    {
        vec3 lightDir;
        float diff;

        if (lightTypes[i] == 0) // Directional
        {
            lightDir = normalize(-lightDirections[i]);
        }
        else
        {
            lightDir = normalize(lightPositions[i] - FragPos);
        }

        diff = max(dot(normal, lightDir), 0.0);

        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = specularStrength * spec * lightColors[i];

        float shadow = 1.0;

        if (lightTypes[i] == 0 && hasShadowMap[i] == 1)
        {
            shadow = ShadowCalculation(
                FragPosLightSpace[i],
                shadowMaps[i]
            );
        }

        vec3 lightContribution =
            (diff + specular) *
            lightColors[i] *
            lightIntensities[i];

        lighting += lightContribution * shadow;
    }

    vec4 texColor = useTexture ? texture(texture1, TexCoord) : vec4(1.0);
    vec4 baseColor = texColor * objectColor;

    vec3 finalRGB =
        baseColor.rgb * (vec3(0.2) + lighting);

    FragColor = vec4(finalRGB, baseColor.a);
}