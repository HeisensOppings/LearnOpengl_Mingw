#include "shader.h"

vector<string> Shader::vertexShaderSources;
vector<string> Shader::fragmentShaderSources;
vector<string> Shader::geometryShaderSources;
bool Shader::CodeOutput;

void Shader::ShaderInit(const string filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        exit(0);
    }

    std::stringstream currentShader;
    std::string line;
    bool inVertexShader = false;
    bool inFragmentShader = false;
    bool inGeometryShader = false;

    while (std::getline(file, line))
    {
        if (line.substr(0, 2) == "//")
        {
            continue;
        }
        if (line.find("#shader vertex") != std::string::npos)
        {
            inVertexShader = true;
        }
        else if (line.find("#shader fragment") != std::string::npos)
        {
            inFragmentShader = true;
        }
        else if (line.find("#shader geometry") != std::string::npos)
        {
            inGeometryShader = true;
        }
        else if (!line.empty())
        {
            currentShader << line << "\n";
        }
        else if (inVertexShader)
        {
            vertexShaderSources.push_back(currentShader.str());
            currentShader.str("");
            currentShader.clear();
            inVertexShader = false;
        }
        else if (inFragmentShader)
        {
            fragmentShaderSources.push_back(currentShader.str());
            currentShader.str("");
            currentShader.clear();
            inFragmentShader = false;
        }
        else if (inGeometryShader)
        {
            geometryShaderSources.push_back(currentShader.str());
            currentShader.str("");
            currentShader.clear();
            inGeometryShader = false;
        }
    }
    if (inVertexShader)
    {
        vertexShaderSources.push_back(currentShader.str());
    }
    if (inFragmentShader)
    {
        fragmentShaderSources.push_back(currentShader.str());
    }
    if (inGeometryShader)
    {
        geometryShaderSources.push_back(currentShader.str());
    }
}

Shader::Shader(unsigned int vertexShader_ID, unsigned int fragmentShader_ID, int geometryShader_ID)
{
    if (vertexShaderSources[vertexShader_ID].empty())
    {
        throw std::runtime_error("vertexShader_ID is empty");
    }
    if (fragmentShaderSources[fragmentShader_ID].empty())
    {
        throw std::runtime_error("fragmentShader_ID is empty");
    }
    if (geometryShader_ID != -1 && geometryShaderSources[geometryShader_ID].empty())
    {
        throw std::runtime_error("geometryShader_ID is empty");
    }
    const char *vertexShaderSource = vertexShaderSources[vertexShader_ID].c_str();
    const char *fragmentShaderSource = fragmentShaderSources[fragmentShader_ID].c_str();
    const char *geometryShaderSource;
    if (geometryShader_ID != -1)
        geometryShaderSource = geometryShaderSources[geometryShader_ID].c_str();

    if (CodeOutput)
    {
        cout << vertexShaderSource << endl;
        cout << fragmentShaderSource << endl;
        if (geometryShader_ID != -1)
            cout << geometryShaderSource << endl;
    }
    unsigned int vertexShader, fragmentShader, geometryShader = 0;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    CHECK_SHADER(vertexShader, vertexShader_ID);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    CHECK_SHADER(fragmentShader, fragmentShader_ID);

    if (geometryShader_ID != -1)
    {
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
        glCompileShader(geometryShader);
        CHECK_SHADER(geometryShader, geometryShader_ID);
    }

    ProgramID = glCreateProgram();
    glAttachShader(ProgramID, vertexShader);
    glAttachShader(ProgramID, fragmentShader);
    if (geometryShader_ID != -1)
        glAttachShader(ProgramID, geometryShader);
    glLinkProgram(ProgramID);
    CHECK_PROGRAM(ProgramID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (geometryShader_ID != -1)
        glDeleteShader(geometryShader);
}

int Shader::GetUniformLocation(const std::string &name)
{
    if (m_UniformLoationCache.find(name) != m_UniformLoationCache.end())
        return m_UniformLoationCache[name];

    int location = glGetUniformLocation(ProgramID, name.c_str());
    if (location == -1)
    {
        std::cout << "No active uniform variable with name " << name << " foune" << std::endl;
    }

    m_UniformLoationCache[name] = location;
    return location;
}

void Shader::SetUniform1b(const std::string &name, bool value)
{
    glUniform1i(GetUniformLocation(name), (int)value);
}

void Shader::SetUniform1i(const std::string &name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string &name, float f0)
{
    glUniform1f(GetUniformLocation(name), f0);
}

void Shader::SetUniform2f(const std::string &name, float f0, float f1)
{
    glUniform2f(GetUniformLocation(name), f0, f1);
}

void Shader::SetUniform2f(const std::string &name, const glm::vec2 &value)
{
    glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetUniform3f(const std::string &name, float f0, float f1, float f2)
{
    glUniform3f(GetUniformLocation(name), f0, f1, f2);
}

void Shader::SetUniform3f(const std::string &name, const glm::vec3 &value)
{
    glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetUniform4f(const std::string &name, float f0, float f1, float f2, float f3)
{
    glUniform4f(GetUniformLocation(name), f0, f1, f2, f3);
}

void Shader::SetUniform4f(const std::string &name, const glm::vec4 &value)
{
    glUniform4fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetUniform2m(const std::string &name, const glm::mat2 &mat)
{
    glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniform3m(const std::string &name, const glm::mat3 &mat)
{
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniform4m(const std::string &name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::Clear()
{
    vertexShaderSources.erase(vertexShaderSources.begin(),vertexShaderSources.end());
    fragmentShaderSources.erase(fragmentShaderSources.begin(),fragmentShaderSources.end());
    geometryShaderSources.erase(geometryShaderSources.begin(),geometryShaderSources.end());
}
