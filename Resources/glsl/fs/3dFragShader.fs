#version 420

out vec4 frag_color;
in vec3 lPos;
in vec2 lTex;
in vec3 lNormal;
uniform vec3 lightPos;

void main()
{
    vec3 L = lightPos-lPos;
    float diffuse = dot(lNormal, normalize(L));
    // float attenuation = clamp( 5.0/length(L), 0.0, 1.0);
    float attenuation = 1.0;

    // float attenuation = clamp( 1.0/(1.0+length(L)+length(L)*length(L)), 0.0, 1.0);

    vec3 diffuseColor = vec3(0.65, 0.65, 0.65)*diffuse*attenuation;
    vec3 ambientColor = vec3(0.25, 0.25, 0.25);
    vec3 finalColor = (diffuseColor+ambientColor);
    // frag_color = vec4(ambientColor+diffuseColor, 1.0);
    frag_color = vec4(lNormal, 1.0);
    // frag_color = vec4(lTex, 0.0, 1.0);
}