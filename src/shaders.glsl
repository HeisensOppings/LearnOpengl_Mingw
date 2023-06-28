#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
out vec3 ourColor;
out vec2 TexCoord;
uniform vec2 xyOffset;
uniform mat4 transform;
void main()
{
    gl_Position = transform * vec4(aPos, 1.0f);
    // gl_Position = transform * vec4(aPos.x + xyOffset.x, aPos.y + xyOffset.y, aPos.z, 1.0f);
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
uniform bool switchTexture;
void main()
{
    vec2 offset = vec2(view_position.x, view_position.y);
    offset = vec2(0.5, 0.5) - 0.5 * scale + offset;
    vec2 scaledTexCoord = TexCoord * scale + offset;
    FragColor = texture(switchTexture?texture1:texture2, scaledTexCoord);
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
uniform vec4 ourColor;
void main()
{
    FragColor=ourColor;
}