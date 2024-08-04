#version 330 core
out vec4 FragColor;

uniform vec3 lightColor;

void main()
{
    FragColor = vec4(lightColor, 1.0); // usar el color de la luz configurado desde el C++
}

