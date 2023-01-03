#version 460 core


layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

out vec2 uv_f;

void main()
{
	// 0.96875
	// 0.984375
	gl_Position = vec4(pos,0.,1.);
	uv_f = uv;
}

