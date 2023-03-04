#version 330 core
out vec4 FragColor;


in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

uniform sampler2D textureSun;
uniform vec3 light;


void main()
{
	vec3 color = light * texture(textureSun, TexCoords).rgb;
	FragColor = vec4(color, 1.0);
}