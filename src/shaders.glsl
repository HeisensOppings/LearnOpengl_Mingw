#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
out vec3 ourColor;
out vec2 TexCoord;
uniform mat4 transform;
void main()
{
    gl_Position = transform * vec4(aPos, 1.0f);
    ourColor = aColor;
    TexCoord = aTexCoord;
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float scale;
uniform vec2 view_position;
uniform int switchTexture;
void main()
{
    vec2 offset = vec2(view_position.x, view_position.y);
    offset = vec2(0.5, 0.5) - 0.5 * scale + offset;
    vec2 scaledTexCoord = TexCoord * scale + offset;
    if(switchTexture == 1)
    FragColor = texture(texture1, scaledTexCoord);
    else
    FragColor = texture(texture2, scaledTexCoord);
}

#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
uniform mat4 transform;
void main()
{
    gl_Position = transform * vec4(aPos, 1.0f);
    ourColor = aColor;
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 ourColor;
void main()
{
    FragColor=vec4(ourColor, 1.0f);
}