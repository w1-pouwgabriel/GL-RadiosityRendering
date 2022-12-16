#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 ObjectColor;
uniform vec3 LightPos;
uniform vec3 LightColor;

void main()
{
    //Caluclate light direction
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);  

    //Calculate diffuse color
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * LightColor;
    
    //Calculate ambient color
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * LightColor;

    vec3 result = (ambient + diffuse) * ObjectColor;

    FragColor = vec4(result,1);
}