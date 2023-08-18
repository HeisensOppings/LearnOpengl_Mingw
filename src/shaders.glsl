#shader vertex
// id --- 0
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float scales;
void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    // normalMatrix = mat3(transpose(inverse(model))) * aNormal;  
    Normal = normalize(normalMatrix * aNormal);
    TexCoords = aTexCoords * scales;
    gl_Position = projection * view * vec4(FragPos, 1.0);
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
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
in vec4 FragPosLightSpace;
uniform vec3 viewPos;
uniform Dirlight dirLight;
uniform Pointlight pointLights[POINT_LIGHTS];
uniform Spotlight spotLight;
uniform Material material;
// uniform sampler2D texture_diffuse1;
// uniform sampler2D texture_specular1;
// uniform sampler2D texture_normal1;
uniform sampler2D depthMapDir;
uniform samplerCube depthMapPoint;
uniform float zbuffer_near;
uniform float zbuffer_far;
uniform float far_plane;
// uniform int depth_test;
// uniform samplerCube skybox;
// uniform float refractive_rate;
uniform int gamma;
vec3 CalcDirLight(Dirlight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Pointlight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float LinearizeDepth(float depth);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
float ShadowCalculation(vec3 fragPos);
void main()
{
    // properties
    // texture_diffuse1;
    // texture_specular1;
    // texture_normal1;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    // if(depth_test == 0)
    // {
        vec3 result = CalcDirLight(dirLight, norm, viewDir);
        for(int i = 0; i < POINT_LIGHTS; ++i)
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
        result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
        // if(gamma == 1)
            // result = pow(result, vec3(1.0/2.2));
        FragColor = vec4(result, 1.0);
        // vec3 I = normalize(FragPos - viewPos);
        // vec3 R = reflect(I, normalize(Normal));
        // FragColor = vec4(texture(skybox, R).rgb, 1.0);
    // }
    // else
    // {
    // FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
        // float depth = LinearizeDepth(gl_FragCoord.z) / zbuffer_far;
        // FragColor = vec4(vec3(depth), 1.0);
        //
        // vec3 I = normalize(FragPos - viewPos);
        // vec3 R = reflect(I, normalize(Normal));
        // FragColor = vec4(texture(skybox, R).rgb, 1.0);
        // 
    //     float ratio = 1.00 / refractive_rate;
    //     vec3 I = normalize(FragPos - viewPos);
    //     vec3 R = refract(I, normalize(Normal), ratio);
    //     FragColor = vec4(texture(skybox, R).rgb, 1.0);
    // }
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
    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);
    return ((ambient + (1.0 - shadow) * (diffuse + specular))* vec3(texture(material.diffuse, TexCoords)));
}
vec3 CalcPointLight(Pointlight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    // blinn
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // phone
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine
    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    // ambient *= attenuation;
    // diffuse *= attenuation;
    // specular *= attenuation;
    float shadow = ShadowCalculation(fragPos);                      
    return((ambient + (1.0 - shadow) * (diffuse + specular)) * vec3(texture(material.diffuse, TexCoords)));
    // return((ambient ) * vec3(texture(material.diffuse, TexCoords)));
}
vec3 CalcSpotLight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    // blinn
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // phone
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    // return ((ambient + diffuse + specular) * vec3(texture(material.diffuse, TexCoords)));
    float shadow = ShadowCalculation(fragPos);                      
    return((ambient + (1.0 - shadow) * (diffuse + specular)) * vec3(texture(material.diffuse, TexCoords)));
}
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(projCoords.z > 1.0){
        return 0.0;}
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    // float closestDepth = texture(depthMapDir, projCoords.xy).r; 
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    // float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    float bias = max(0.02 * (1.0 - dot(normal, lightDir)), 0.002);
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMapDir, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMapDir, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;
}
float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - pointLights[0].position;
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMapPoint, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
    return shadow;
}

#shader vertex
// id --- 1
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
// id --- 2
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
// id --- 3
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
// id --- 4
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

#shader vertex
// id --- 5
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 3) in mat4 aInstanceMatrix;
out VS_OUT {
    vec3 normal;
} vs_out;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 projection;
void main()
{
    // mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    // vs_out.normal = vec3(vec4(normalMatrix * aNormal, 0.0));
    vs_out.normal = normalize( mat3(transpose(inverse(aInstanceMatrix))) * aNormal);
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0); 
}

#shader geometry
#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 3) out;
in VS_OUT {
    vec3 normal;
} gs_in[];
out vec3 fColor;
const float MAGNITUDE = 0.01;
uniform float time;
void GenerateLine(int index)
{
    float direction = ((sin(time) + 1.0) / 10.0); 
    fColor = vec3(0.0f, 1.0f, 0.0f);
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = (gl_in[index].gl_Position  + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    fColor = vec3(1.0f, 0.0f, 0.0f);
    gl_Position = (gl_in[index].gl_Position  + vec4(gs_in[index].normal, 0.0) * MAGNITUDE + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}
void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec3 fColor;
void main()
{
    FragColor = vec4(fColor, 1.0);
}

#shader vertex
// id --- 6
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 lightSpaceMatrix;
uniform mat4 model;
void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core
void main()
{             
    // gl_FragDepth = gl_FragCoord.z;
}

#shader vertex
// id --- 7
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
out vec2 TexCoords;
void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D depthMap;
uniform float near_plane;
uniform float far_plane;
// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}
void main()
{             
    float depthValue = texture(depthMap, TexCoords).r;
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}

#shader vertex
// id --- 8
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
void main()
{
    gl_Position = model * vec4(aPos, 1.0);
}

#shader geometry
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;
uniform mat4 shadowMatrices[6];
out vec4 FragPos; // FragPos from GS (output per emitvertex)
void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
} 

#shader fragment
#version 330 core
in vec4 FragPos;
uniform vec3 lightPos;
uniform float far_plane;
void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    // write this as modified depth
    gl_FragDepth = lightDistance;
}

#shader vertex
// id --- 8
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
out vec2 TexCoords;
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform bool reverse_normals;
void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;
uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float far_plane;
uniform bool shadows;
float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
    return shadow;
}
void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = shadows ? ShadowCalculation(fs_in.FragPos) : 0.0;                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    FragColor = vec4(lighting, 1.0);
}

// #shader vertex
// #version 330 core
// layout (location = 0) in vec3 aPos;
// layout (location = 2) in vec2 aTexCoords;
// layout (location = 3) in mat4 aInstanceMatrix;
// out vec2 TexCoords;
// uniform mat4 projection;
// uniform mat4 view;
// void main()
// {
//     TexCoords = aTexCoords;
//     gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0f); 
// }

// #shader fragment
// #version 330 core
// out vec4 FragColor;
// in vec2 TexCoords;
// uniform sampler2D texture_diffuse1;
// void main()
// {
//     FragColor = texture(texture_diffuse1, TexCoords);
// }

// #shader geometry
// #version 330 core
// layout (triangles) in;
// layout (triangle_strip, max_vertices = 3) out;
// in VS_OUT {
//     vec2 texCoords;
//     vec3 fragPos;
//     vec3 normal;
// } gs_in[];
// out vec3 FragPos;
// out vec3 Normal;
// out vec2 TexCoords; 
// uniform float time;
// vec4 explode(vec4 position, vec3 normal)
// {
//     // float magnitude = 1.0;
//     // vec3 direction = normal * ((sin(time) + 1.0) / 10.0); 
//     // vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
//     // return position + vec4(direction, 0.0);
//     return position + vec4(normal*0.001, 0.0);
//     // return position;
// }
// vec3 GetNormal()
// {
//     vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
//     vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
//     return normalize(cross(a, b));
// }
// void main() {    
//     vec3 normal = GetNormal();
//     // gl_Position = explode(gl_in[0].gl_Position, normal);
//     gl_Position = explode(gl_in[0].gl_Position, normal);
//     TexCoords = gs_in[0].texCoords;
//     FragPos = gs_in[0].fragPos;
//     Normal = gs_in[0].normal;
//     EmitVertex();
//     gl_Position = explode(gl_in[1].gl_Position, normal);
//     TexCoords = gs_in[1].texCoords;
//     FragPos = gs_in[1].fragPos;
//     Normal = gs_in[1].normal;
//     EmitVertex();
//     gl_Position = explode(gl_in[2].gl_Position, normal);
//     TexCoords = gs_in[2].texCoords;
//     FragPos = gs_in[2].fragPos;
//     Normal = gs_in[2].normal;
//     EmitVertex();
//     EndPrimitive();
// }
