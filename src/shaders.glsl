#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float scales;
void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    // Normal = mat3(transpose(inverse(model))) * aNormal;  
    Normal = normalize(normalMatrix * aNormal);
    gl_Position = projection * view * vec4(FragPos, 1.0);
    TexCoords = aTexCoords * scales;
}

#shader fragment
#version 330 core
out vec4 FragColor;
struct Material{
    sampler2D diffuse;
    sampler2D specular;
    int shininess;
};
struct Dirlight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct Pointlight{
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct Spotlight{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define POINT_LIGHTS 1
in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
uniform vec3 viewPos;
uniform Dirlight dirLight;
uniform Pointlight pointLights[POINT_LIGHTS];
uniform Spotlight spotLight;
uniform Material material;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform float zbuffer_near;
uniform float zbuffer_far;
uniform int depth_test;
uniform samplerCube skybox;
uniform float refractive_rate;
vec3 CalcDirLight(Dirlight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Pointlight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float LinearizeDepth(float depth);
void main()
{
    // properties
    texture_diffuse1;
    texture_specular1;
    texture_normal1;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    if(depth_test == 0)
    {
        // vec3 result = CalcDirLight(dirLight, norm, viewDir);
        // for(int i = 0; i < POINT_LIGHTS; ++i)
        //     result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
        // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
        // FragColor = vec4(result, 1.0);
        vec3 I = normalize(FragPos - viewPos);
        vec3 R = reflect(I, normalize(Normal));
        FragColor = vec4(texture(skybox, R).rgb, 1.0);
    }
    else
    {
    // FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
        // float depth = LinearizeDepth(gl_FragCoord.z) / zbuffer_far;
        // FragColor = vec4(vec3(depth), 1.0);
        //
        // vec3 I = normalize(FragPos - viewPos);
        // vec3 R = reflect(I, normalize(Normal));
        // FragColor = vec4(texture(skybox, R).rgb, 1.0);
        // 
        float ratio = 1.00 / refractive_rate;
        vec3 I = normalize(FragPos - viewPos);
        vec3 R = refract(I, normalize(Normal), ratio);
        FragColor = vec4(texture(skybox, R).rgb, 1.0);
    }
} 
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; //back to NDC (Normalized Device Coordinates)
    return (2.0 * zbuffer_far * zbuffer_near) / (zbuffer_far + zbuffer_near - z * (zbuffer_far - zbuffer_near));
}
vec3 CalcDirLight(Dirlight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}
vec3 CalcPointLight(Pointlight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return(ambient + diffuse + specular);
}
vec3 CalcSpotLight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;
uniform vec3 lightColor;
void main()
{
    FragColor = vec4(lightColor,1.0);
}

#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 TexCoords;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D texture1;
void main()
{             
    vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.01)
        discard;
    else
        FragColor = texColor;
}

#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
out vec2 TexCoords;
void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
}  

#shader fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
const float offset = 1.0 / 300.0; 
uniform int mode;
vec3 kernel_effects(int mode);
void main()
{
    vec3 col = vec3(0.0);
    switch(mode)
    {
    // --- normal
        case 0:
            col = texture(screenTexture, TexCoords).xyz;
        break;
    // --- inversion
        case 1:
            col = vec3(1.0f - texture(screenTexture, TexCoords).xyz);
        break;
    // --- Grayscale
        case 2:
            col = texture(screenTexture, TexCoords).xyz;
            float average = (col.r + col.g + col.b) / 3.0;
            col = vec3(average, average, average);
        break;
    // --- sharpen
        case 3:
            col = kernel_effects(3);
        break;
    // --- blur
        case 4:
            col = kernel_effects(4);
        break;
    // --- edge detection
        case 5:
            col = kernel_effects(5);
        break;
        default:
            col = vec3(1.0f, 1.0f, 1.0f);
        break;
    }
    FragColor = vec4(col, 1.0);
}
vec3 kernel_effects(int mode)
{
    // --- kernel effects
        vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );
    float kernel[9];
    if (mode == 3)
    {   
    // --- sharpen
        kernel = float[](
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1
        );
    }
    else if(mode == 4)
    {
    // --- blur
        kernel = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
        );
    }
    else if(mode == 5)
    {
    // --- edge detection
        kernel = float[](
            1, 1, 1,
            1, -8, 1,
            1, 1, 1
        );
    }
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    return col;
    // FragColor = vec4(col, 1.0);
    // float average = (col.r + col.g + col.b) / 3.0;
    // FragColor = vec4(average, average, average, 1.0);
}

#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 TexCoords;
uniform mat4 projection;
uniform mat4 view;
void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 TexCoords;
uniform samplerCube skybox;
void main()
{    
    FragColor = texture(skybox, TexCoords);
}