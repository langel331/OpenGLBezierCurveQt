#version 440

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

out vec3 theColor;

void main()
{
	vec2 pos = vec2(position.x, position.y);
	gl_Position = vec4(pos, 0.0, 1.0);
	theColor = vertexColor;
}