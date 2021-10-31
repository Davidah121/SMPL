#version 420

in vec3 vertNormal;
in vec3 vertPos;

uniform vec3 lightPos;
uniform vec3 lightCol;
uniform float maxDis;
uniform vec3 ambientCol;

out vec4 fragColor;

void main()
{
    vec3 toPos = vertPos - lightPos;
    vec3 toPosN = normalize(toPos);
    vec3 vertN = normalize(vertNormal);

    float disToLight = length(toPos);
    float attenuation = clamp(maxDis / disToLight, 0.0, 1.0);

    float lightIntensity = dot(toPosN, vertN);
    
    vec3 diffuse = lightCol*lightIntensity*attenuation;
    diffuse = clamp(diffuse, 0.0, 1.0);

    vec3 finalColor = ambientCol + diffuse;

    finalColor = clamp(finalColor, 0.0, 1.0);

    fragColor = vec4(finalColor, 1.0);
}
