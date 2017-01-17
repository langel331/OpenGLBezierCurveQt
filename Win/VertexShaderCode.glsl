#version 440

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

out vec3 theColor;

void main()
{
	gl_Position = vec4(position, 7.0);
	theColor = vertexColor;
	//theColor = vec3(0.0, 1.0, 0.5);
}