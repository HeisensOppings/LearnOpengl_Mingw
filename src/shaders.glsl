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
void main()
{
    // vec2 offset = vec2(view_position.x, view_position.y);
    // offset = vec2(0.5, 0.5) - 0.5 * scale + offset;
    // vec2 scaledTexCoord = TexCoord * scale + offset;
    // vec4 objectTexture = vec4(1.0);
    // if(switchTexture == 1)
    // objectTexture = texture(texture1, scaledTexCoord);
    // else
    // objectTexture = texture(texture2, scaledTexCoord);
    FragPos = vec3(model * vec4(aPos, 1.0));
    // Normal = mat3(transpose(inverse(model))) * aNormal;  
    Normal = normalize(normalMatrix * aNormal);
    gl_Position = projection * view * vec4(FragPos, 1.0);
    TexCoords = aTexCoords;
}

#shader fragment
#version 330 core
out vec4 FragColor;
struct Material{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    // vec3 ambient;
    // vec3 diffuse;
    // vec3 specular;
    float shininess;
};
struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform float yoffset;
void main()
{
    // ambient
    // float ambientStrength = 0.2;
    // vec3 ambient = ambientStrength * lightColor;
    // vec3 ambient = light.ambient * material.ambient;
    // vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    // vec3 diffuse = diff * lightColor;
    // vec3 diffuse = light.diffuse * (diff * texture(material.diffuse, TexCoords).rgb);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // specular
    // float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    // vec3 specular = specularStrength * spec * lightColor;
    // vec3 specular = light.specular * (spec * material.specular);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));// inverse the sampled specular color.
    // vec3 specular = light.specular * spec * (vec3(1.0) -vec3(texture(material.specular, TexCoords)));// inverse the sampled specular color.
    // emission
    vec3 emission = vec3(0.0);
    if(vec3(texture(material.specular, TexCoords)) == emission)
    {
        emission = texture(material.emission, TexCoords + vec2(0.0,yoffset)).gbr;
        emission = emission * (sin(yoffset) * 0.5 + 0.5) * 2.0;
    }
    // result
    FragColor = vec4(ambient + diffuse + specular + emission, 1.0);
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
    FragColor = vec4(lightColor,0.0);
}