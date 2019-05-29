#version 330 core
out vec4 FragColor;

uniform bool red;

void main()
{
	if (red == true)
    	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    else
    	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}