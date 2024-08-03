#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture1;

uniform vec3 lightDir; // Dirección de la luz (uniforme para todos los objetos)
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // Ambiente
    float ambientStrength = 0.8;
    vec3 ambient = ambientStrength * lightColor;

    // Difuso
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir); // La luz direccional entra en la escena desde esta dirección
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    // Especular
    float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = texture(texture1, TexCoord) * vec4(result, 1.0);
}
