#version 330 core
out vec4 FragColor;

uniform int color;

void main() {
	if (color == 0)
	{
		FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Color rojo para las líneas
	}
	else if (color == 1)
	{
		FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Color verde para las líneas
	}
	else if (color == 2)
	{
		FragColor = vec4(0.0, 0.0, 1.0, 1.0); // Color azul para las líneas
	}    
}
