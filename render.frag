#version 460 core


in vec2 uv_f;

layout(location = 0) uniform sampler2D tex;

out vec4 fragColor;

void main()
{
	fragColor = texture(tex,uv_f);
}

