#version 330 core
out vec4 FragColor;

// Estructura para definir las propiedades del material
struct Material {
    sampler2D diffuse;   // Textura para el color difuso
    sampler2D specular;  // Textura para el color especular
    float shininess;     // Brillo del material
};

// Estructura para definir una luz direccional
struct DirLight {
    vec3 direction;      // Dirección de la luz

    vec3 ambient;        // Componente de luz ambiental
    vec3 diffuse;        // Componente de luz difusa
    vec3 specular;       // Componente de luz especular
};

// Estructura para definir una luz puntual
struct PointLight {
    vec3 position;       // Posición de la luz

    vec3 ambient;        // Componente de luz ambiental
    vec3 diffuse;        // Componente de luz difusa
    vec3 specular;       // Componente de luz especular

    float constant;      // Factor de atenuación constante
    float linear;        // Factor de atenuación lineal
    float quadratic;     // Factor de atenuación cuadrático
};

#define NR_POINT_LIGHTS 4  // Número máximo de luces puntuales

// Variables de entrada del shader
in vec3 FragPos;        // Posición del fragmento
in vec3 Normal;         // Normal del fragmento
in vec2 TexCoords;     // Coordenadas de textura

// Variables uniformes (definidas por el código de la aplicación)
uniform vec3 viewPos;              // Posición del observador
uniform DirLight dirLight;         // Luz direccional principal (como la fogata)
uniform PointLight pointLights[NR_POINT_LIGHTS]; // Array de luces puntuales
uniform Material material;         // Propiedades del material
uniform DirLight moonLight;        // Luz direccional secundaria (como la luna)

// Prototipos de funciones
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // Normaliza las normales y el vector de vista
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Fase 1: Calcula la iluminación de la luz direccional (fogata)
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    // Fase 2: Calcula la iluminación de la luz direccional secundaria (luna)
    result += CalcDirLight(moonLight, norm, viewDir);

    // Fase 3: Calcula la iluminación de las luces puntuales
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    // Establece el color final del fragmento
    FragColor = vec4(result, 1.0);
}

// Calcula la iluminación para una luz direccional
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    // Calcula la dirección de la luz (invertida, ya que es direccional)
    vec3 lightDir = normalize(-light.direction);

    // Iluminación difusa
    float diff = max(dot(normal, lightDir), 0.0);

    // Iluminación especular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Combina resultados de luz ambiental, difusa y especular
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

// Calcula la iluminación para una luz puntual
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Calcula la dirección de la luz
    vec3 lightDir = normalize(light.position - fragPos);

    // Iluminación difusa
    float diff = max(dot(normal, lightDir), 0.0);

    // Iluminación especular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Calcula la distancia y la atenuación de la luz puntual
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Combina resultados de luz ambiental, difusa y especular con atenuación
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

