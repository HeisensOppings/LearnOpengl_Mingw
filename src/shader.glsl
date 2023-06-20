#shader vertex
layout (location = 0) in vec3 aPos;   // 位置变量的属性位置值为 0 
layout (location = 1) in vec3 aColor; // 颜色变量的属性位置值为 1
out vec3 ourColor; // 向片段着色器输出一个颜色
uniform vec2 xyOffset;
void main()
{
    gl_Position = vec4(aPos.x + xyOffset.x, aPos.y + xyOffset.y, aPos.z, 1.0f);
    ourColor = vec3(aColor.x + xyOffset.x, aColor.y + xyOffset.y, aColor.z); // 将ourColor设置为我们从顶点数据那里得到的输入颜色
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
void main()
{
    FragColor=vec4(ourColor, 1.0f);
}

#shader fragment
#version 330 core
out vec4 FragColor;
uniform vec4 ourColor;
void main()
{
    FragColor=ourColor;
}