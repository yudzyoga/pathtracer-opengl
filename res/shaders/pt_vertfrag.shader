#shader vertex
#version 460 core
layout(location=0) in vec4 position;						   
layout(location=1) in vec2 texCoord;	
out vec2 v_TexCoord;
void main()
{
    gl_Position = position;
    v_TexCoord = texCoord;
};



#shader fragment
#version 460 core		
uniform sampler2D u_Texture;
in vec2 v_TexCoord;
out vec4 texColor;
void main()
{
    vec4 texColor = texture(u_Texture, v_TexCoord);
};

