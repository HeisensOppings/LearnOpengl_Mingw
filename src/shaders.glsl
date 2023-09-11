#shader vertex
// id --- 0
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
// layout (location = 4) in vec3 aBitangent;
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpaceDir;
out vec4 FragPosLightSpaceSpot;
out vec3 TangentLightPos;
out vec3 TangentSpotLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;
uniform mat4 lightSpaceMatrixSpot;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int hasNormalMap;
uniform vec3 pointLightPos;
uniform vec3 spotLightPos;
uniform vec3 dirLightDirecation;
uniform vec3 spotLightDirecation;
out vec3 DirLightDirecation;
out vec3 SpotLightDirecation;
void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    // normalMatrix = mat3(transpose(inverse(model))) * aNormal;  
    gl_Position = projection * view * vec4(FragPos, 1.0);
    FragPosLightSpaceDir = lightSpaceMatrix * vec4(FragPos, 1.0);
    FragPosLightSpaceSpot = lightSpaceMatrixSpot * vec4(FragPos, 1.0);
    if(hasNormalMap == 1)
    {
        vec3 T = normalize(normalMatrix * aTangent);
        vec3 N = normalize(normalMatrix * aNormal);
        T = normalize(T - dot(T, N) * N);
        vec3 B = cross(N, T);
        mat3 TBN = transpose(mat3(T, B, N)); 
        TangentLightPos = TBN * pointLightPos;
        TangentSpotLightPos = TBN * spotLightPos;
        TangentViewPos  = TBN * viewPos;
        TangentFragPos  = TBN * FragPos;
        DirLightDirecation = TBN * dirLightDirecation;
        SpotLightDirecation = TBN * spotLightDirecation;
    }
    else 
    {
        TangentLightPos = pointLightPos;
        TangentSpotLightPos = spotLightPos;
        TangentViewPos  = viewPos;
        TangentFragPos  = FragPos;
        DirLightDirecation = dirLightDirecation;
        SpotLightDirecation = spotLightDirecation;
        Normal = normalize(normalMatrix * aNormal);
    }
}

#shader fragment
#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
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
in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
in vec4 FragPosLightSpaceDir;
in vec4 FragPosLightSpaceSpot;
in vec3 TangentSpotLightPos;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
uniform vec3 viewPos;
uniform vec3 spotLightPos;
uniform Dirlight dirLight;
uniform Pointlight pointLight;
uniform Spotlight spotLight;
uniform Material material;
uniform sampler2D depthMapDir;
uniform samplerCube depthMapPoint;
uniform sampler2D depthMapSpot;
uniform sampler2D normalMap;
uniform sampler2D heightMap;
uniform float far_plane;
uniform int hasNormalMap;
// uniform int hasHeightMap;
in vec3 DirLightDirecation;
in vec3 SpotLightDirecation;
vec3 CalcDirLight(Dirlight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Pointlight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculationDir(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
float ShadowCalculationPoint();
float ShadowCalculationSpot(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
vec2 ParallaxMapping(vec2 TexCoords, vec3 viewDir);
vec2 texCoords;
void main()
{
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    // if(hasHeightMap == 1)
    // {   
    //     texCoords = ParallaxMapping(TexCoords, viewDir);
    //     if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
    //         discard;
    // }
    // else
        texCoords = TexCoords;
    vec3 norm;
    if(hasNormalMap == 1)
    {
        norm = texture(normalMap, texCoords).rgb;
        norm = normalize(norm * 2.0 - 1.0);  // this normal is in tangent space
    }
    else 
        norm = Normal;
    // vec3 result = CalcDirLight(dirLight, norm, viewDir);
    vec3 result = CalcPointLight(pointLight, norm, FragPos, viewDir);
    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    FragColor = vec4(result * vec3(texture(material.diffuse, texCoords)), 1.0);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(result * vec3(texture(material.diffuse, texCoords)), 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    if (dot(normalize(texture(material.diffuse, texCoords).rgb), normalize(vec3(198.0, 66.0, 116.0))) == 1.0)
    {
        FragColor = vec4(vec3(198.0, 66.0, 116.0) / 50, 1.0);
        BrightColor = FragColor;
    }
}
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy * 0.1;
    vec2 deltaTexCoords = P / numLayers;
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(heightMap, currentTexCoords).r;
    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = (texture(heightMap, currentTexCoords).r);
        currentLayerDepth += layerDepth;
    }
    // float height = texture(heightMap, texCoords).r;
    // return texCoords - viewDir.xy * (height * 0.1);
    // return currentTexCoords;
        // get texture coordinates before collision (reverse operations)
        vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
        // get depth after and before collision for linear interpolation
        float afterDepth  = currentDepthMapValue - currentLayerDepth;
        float beforeDepth = (texture(heightMap, prevTexCoords).r) - currentLayerDepth + layerDepth;
        // interpolation of texture coordinates
        float weight = afterDepth / (afterDepth - beforeDepth);
        vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
        return finalTexCoords;  
}
vec3 CalcDirLight(Dirlight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(DirLightDirecation);
    // vec3 lightDir = normalize(light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine
    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    float shadow = ShadowCalculationDir(FragPosLightSpaceDir, normal, lightDir);
    return ((ambient + (1.0 - shadow) * (diffuse + specular)));
    // return (ambient + diffuse + specular);
}
vec3 CalcPointLight(Pointlight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    // blinn
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(TangentLightPos - TangentFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine
    vec3 ambient = light.ambient * attenuation;
    vec3 diffuse = light.diffuse * diff * attenuation;
    vec3 specular = light.specular * spec * attenuation;
    float shadow = ShadowCalculationPoint();                      
    return((ambient + (1.0 - shadow) * (diffuse + specular)));
    // return(ambient + diffuse + specular);
}
vec3 CalcSpotLight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(TangentSpotLightPos - TangentFragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    // blinn
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(TangentSpotLightPos - TangentFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-SpotLightDirecation)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * attenuation * intensity;
    vec3 diffuse = light.diffuse * diff * attenuation * intensity;
    vec3 specular = light.specular * spec * attenuation * intensity;
    float shadow = ShadowCalculationSpot(FragPosLightSpaceSpot, normal, lightDir);                      
    return((ambient + (1.0 - shadow) * (diffuse + specular)));
    // return(ambient + diffuse + specular);
}
float ShadowCalculationDir(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(projCoords.z > 1.0){
        return 0.0;}
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float bias = max(0.02 * (1.0 - dot(normal, lightDir)), 0.002);
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
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);
float ShadowCalculationPoint()
{
    vec3 fragToLight = FragPos - pointLight.position;
    float currentDepth = length(fragToLight);
    // 20 points
    float shadow = 0.0;
    float bias = 0.05;
    int samples = 20;
    float closestDepth;
    float viewDistance = length(TangentViewPos - TangentFragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        closestDepth = texture(depthMapPoint, fragToLight + gridSamplingDisk[i] * 0.1 * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
    return shadow;
}
float ShadowCalculationSpot(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // float near = 0.1, far = 20.0;
    // (2.0 * near) / (far + near - projCoords.z * (far - near))
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float linearDepth_current = 0.2 / (20.1 - projCoords.z * 19.9);
    float bias = max(0.002 * (1.0 - dot(normal, lightDir)), 0.0002);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMapSpot, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float linearDepth_close = 0.2 / (20.1 - texture(depthMapSpot, projCoords.xy + vec2(x, y) * 0.5 * texelSize).r * 19.9);
            shadow += linearDepth_current - bias> linearDepth_close ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
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
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
uniform vec3 lightColor;
void main()
{
    FragColor = vec4(lightColor, 1.0);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
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
uniform float exposure;
uniform int hdr;
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
            col = texture(screenTexture, TexCoords).rgb;
            // vec3 result = col / (col + vec3(1.0));
            if(hdr == 1)
            {
                // vec3 result = col / (col + vec3(1.0));
                vec3 result = vec3(1.0) - exp(-col * exposure);
                // result = pow(result, vec3(1.0 / 2.0));
                col = result;
            }
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
// id --- 9
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
void main()
{           
}

#shader vertex
// id --- 10 bloom blur
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
uniform sampler2D image;
uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);
void main()
{
    vec2 tex_offset = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, TexCoords).rgb * weight[0];
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}

#shader vertex
// id --- 11 bloom final
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
uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;
void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    if(bloom)
        hdrColor += bloomColor;
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    FragColor = vec4(result, 1.0);
}