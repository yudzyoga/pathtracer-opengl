#shader vertex
#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
out vec2 v_TexCoord;
void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
	v_TexCoord = texCoord;
}
    

#shader fragment
#version 460 core
out vec4 f_FragColor;
uniform sampler2D u_Texture;
in vec2 v_TexCoord;
void main()
{
	f_FragColor = texture(u_Texture, v_TexCoord);
}
