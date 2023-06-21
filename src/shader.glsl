#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
out vec3 ourColor;
out vec2 TexCoord;
uniform vec2 xyOffset;
void main()
{
    gl_Position = vec4(aPos.x + xyOffset.x, aPos.y + xyOffset.y, aPos.z, 1.0f);
    ourColor = aColor;
    TexCoord = aTexCoord;
}

#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float alpha;
void main()
{
    FragColor = mix(texture(texture1, alpha *TexCoord), texture(texture2, vec2(1.0-TexCoord.x, TexCoord.y)), 0.2);
}

#shader fragment
#version 330 core
out vec4 FragColor;
uniform vec4 ourColor;
void main()
{
    FragColor=ourColor;
}