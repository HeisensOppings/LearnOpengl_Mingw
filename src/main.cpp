#include "main.h"

#define SCR_WIDTH_DEFAULT 800
#define SCR_HEIGHT_DEFAULT 600

int SCR_WIDTH = SCR_WIDTH_DEFAULT;
int SCR_HEIGHT = SCR_HEIGHT_DEFAULT;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool rightClick = true;
bool firstMouse = true;
bool controlKey = false;

Camera camera;
GLFWwindow *window = nullptr;

void renderSphere();
void RenderText(Shader &s, string text, float x, float y, float scale, vec3 color);
int RenderTextUniCode(Shader &shader, u32string u32text, float x, float y, float scale, vec3 color)
{
    static map<FT_ULong, Character> Characters;
    static bool first = true;
    if (first || text_changed)
    {
        // if (!first || text_changed)
        // {
        // for (auto ch : Characters)
        // glDeleteTextures(1, &ch.TextureID);
        // Characters_.clear();
        // }
        first = false;
        text_changed = false;

        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return -1;
        }

        vector<FT_Face> faces;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        for (const string &font_path : font_paths)
        {
            FT_Face face;
            if (FT_New_Face(ft, font_path.c_str(), 0, &face))
            {
                cout << "ERROR::FREETYPE: Failed to load font" << font_path << std::endl;
                return -1;
            }
            FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);
            faces.push_back(face);
        }

        // std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        // std::wstring wide_str = converter.from_bytes(text_str_);
        // std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
        // std::u32string wide_str = converter.from_bytes(text_str_);

        for (size_t i = 0; i < u32text.length(); i++)
        {
            for (const auto &face : faces)
            {
                FT_UInt glyphIndex = FT_Get_Char_Index(face, u32text[i]);
                if (glyphIndex == 0)
                    continue;
                else
                {
                    FT_Load_Char(face, u32text[i], FT_LOAD_RENDER);
                    // for SDF ----------------------------------------------
                    FT_GlyphSlot slot = face->glyph;
                    FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);
                    // ------------------------------------------------------

                    Character character;
                    if (u32text[i] == ' ')
                    {
                        character.Advance = static_cast<unsigned int>((face)->glyph->advance.x);
                        Characters.insert(std::pair<FT_ULong, Character>(u32text[i], character));
                    }
                    else
                    {
                        unsigned int texture;
                        glGenTextures(1, &texture);
                        glBindTexture(GL_TEXTURE_2D, texture);
                        glTexImage2D(
                            GL_TEXTURE_2D,
                            0,
                            GL_RED,
                            (face)->glyph->bitmap.width,
                            (face)->glyph->bitmap.rows,
                            0,
                            GL_RED,
                            GL_UNSIGNED_BYTE,
                            (face)->glyph->bitmap.buffer);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        character = {
                            texture,
                            glm::ivec2((face)->glyph->bitmap.width, (face)->glyph->bitmap.rows),
                            glm::ivec2((face)->glyph->bitmap_left, (face)->glyph->bitmap_top),
                            static_cast<unsigned int>((face)->glyph->advance.x)};
                        Characters.insert(std::pair<FT_ULong, Character>(u32text[i], character));
                    }
                    // if (u32text[i] == 'm')
                    // {
                    //     for (unsigned int r = 0; r < face->glyph->bitmap.rows; ++r)
                    //     {
                    //         cout << endl;
                    //         for (unsigned int x = 0; x < face->glyph->bitmap.width; ++x)
                    //         {
                    //             unsigned int index = r * face->glyph->bitmap.width + x;
                    //             unsigned int value = face->glyph->bitmap.buffer[index];
                    //             // cout << setw(4)<<(value > 255/2 ? '-' : value)<< " ";
                    //             cout << setw(4) << (value) << " ";
                    //             // cout << (value < 10 ? '-' : '*') << " ";
                    //         }
                    //     }
                    //     cout << endl
                    //          << face->glyph->advance.x << " " << face->glyph->advance.y << endl;
                    //     cout << face->glyph->bitmap.width << " " << face->glyph->bitmap.rows << endl;
                    //     cout << face->glyph->bitmap_left << " " << face->glyph->bitmap_top << endl;
                    //     cout << (face->size->metrics.height >> 6) << endl;
                    // }
                    break;
                }
            }
        }
        for (auto &face : faces)
        {
            FT_Done_Face(face);
        }
        FT_Done_FreeType(ft);
    }
    shader.Bind();
    shader.SetUniform3f("texColor", color);
    shader.SetUniform1i("SDF_Mode", SDF_Mode);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    for (auto c : u32text)
    {
        static float x_pos = x;
        Character &ch = Characters[c];
        if (c == '\n')
        {
            y -= (FONT_SIZE + line_spec) * scale;
            x = x_pos;
        }
        else
        {
            if (c != ' ')
            {
                float xpos = x + ch.Bearing.x * scale;
                float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
                float w = ch.Size.x * scale;
                float h = ch.Size.y * scale;
                // update VBO for each character
                float vertices[4][4] = {
                    {xpos, ypos, 0.0f, 1.0f},
                    {xpos, ypos + h, 0.0f, 0.0f},
                    {xpos + w, ypos, 1.0f, 1.0f},
                    {xpos + w, ypos + h, 1.0f, 0.0f}};

                glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                // update content of VBO memory
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    return 0;
}

int main()
{
    if (opengl_init() != 0)
    {
        cout << "Failed to initialize OpenGL" << endl;
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    ImWchar ranges[] = {0x0020, 0xffff, 0};
    ImWchar ranges1[] = {0x06B0, 0x06ff, 0};
    ImWchar ranges2[] = {0x1550, 0x15DD, 0};
    ImWchar ranges3[] = {0x12199, 0x1219A, 0};
    ImWchar ranges4[] = {0x25E0, 0x28ff, 0};
    ImWchar ranges5[] = {0x02E0, 0x02FF, 0};
    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF("./src/fonts/HarmonyOS_Sans_SC_Medium.ttf", 18.0f, NULL, ranges);
    // io.Fonts->AddFontFromFileTTF("./src/fonts/HarmonyOS_Sans_SC_Medium.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    // io.Fonts->AddFontFromFileTTF("./src/fonts/HarmonyOS_Sans_SC_Medium.ttf", 18.0f, &config, io.Fonts->GetGlyphRangesJapanese());
    io.Fonts->AddFontFromFileTTF("./src/fonts/NotoSansArabic-Medium.ttf", 18.0f, &config, ranges1);             // ڼ
    io.Fonts->AddFontFromFileTTF("./src/fonts/NotoSansCanadianAboriginal-Medium.ttf", 18.0f, &config, ranges2); // ᕕᕗ
    io.Fonts->AddFontFromFileTTF("./src/fonts/NotoSansCuneiform-Regular.ttf", 22.0f, &config, ranges3);         // 𒆚𒆙
    io.Fonts->AddFontFromFileTTF("./src/fonts/NotoSansSymbols2-Regular.ttf", 22.0f, &config, ranges4);          // ⌒
    io.Fonts->AddFontFromFileTTF("./src/fonts/NotoSans-ExtraBold.ttf", 22.0f, &config, ranges5);                // ˰
    ImGui::StyleColorsDark();

    const char *glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Shader::ShaderInit("./src/shaders.glsl");
    Shader Program_cubes(0);
    // Shader Program_light(1);
    Shader Program_buffe(3);
    Shader shaderSKY(4);
    // Shader Program_Depth(6);
    // Shader Program_DCube(8, 8, 1);
    // Shader Program_DSpot(9);
    // Shader ShaderBlur(10);
    // Shader ShaderBloom(11);
    // Shader shaderGeometryPass(12);
    // Shader shaderLightingPass(13);
    Shader shaderLightBox(14);
    // Shader shaderSSAO(15);
    // Shader shaderSSAOBlur(15, 16);
    // Shader::CodeOutput = true;
    Shader shaderPBR(16, 17);
    Shader shaderPBRtexture(16, 18);
    Shader shaderHDR(17, 19);
    Shader shaderIrradiance(17, 20);
    Shader shaderPrefilter(17, 21);
    Shader shaderBRDF(18, 22);
    // Shader::CodeOutput = true;
    Shader shaderTEXT(19, 23);
    // Shader Program_geome(5, 5, 0);
    // Shader::CodeOutput = false;

    Texture T_albedoMap("./src/image/pbr/rusted_iron/albedo.png", GL_TEXTURE0);
    Texture T_normalMap("./src/image/pbr/rusted_iron/normal.png", GL_TEXTURE1);
    Texture T_metallicMap("./src/image/pbr/rusted_iron/metallRoughness.png", GL_TEXTURE2);
    // Texture T_roughnessMap("./src/image/pbr/rusted_iron/metallRoughness.png", GL_TEXTURE3);
    Texture T_aoMap("./src/image/pbr/rusted_iron/ao.png", GL_TEXTURE4);

    shaderPBR.Bind();
    shaderPBR.SetUniform1i("irradianceMap", 0);
    shaderPBR.SetUniform1i("prefilterMap", 1);
    shaderPBR.SetUniform1i("brdfLUT", 2);
    shaderPBR.SetUniform3f("albedo", sphereColor);
    shaderPBR.SetUniform1f("ao", 1.0f);

    shaderPBRtexture.Bind();
    shaderPBRtexture.SetUniform1i("albedoMap", 0);
    shaderPBRtexture.SetUniform1i("normalMap", 1);
    shaderPBRtexture.SetUniform1i("metallicMap", 2);
    shaderPBRtexture.SetUniform1i("roughnessMap", 3);
    shaderPBRtexture.SetUniform1i("aoMap", 4);
    shaderPBRtexture.SetUniform1i("emissiveMap", 5);
    shaderPBRtexture.SetUniform1i("irradianceMap", 6);
    shaderPBRtexture.SetUniform1i("prefilterMap", 7);
    shaderPBRtexture.SetUniform1i("brdfLUT", 8);

    shaderSKY.Bind();
    shaderSKY.SetUniform1i("skybox", 0);

    Program_buffe.Bind();
    Program_buffe.SetUniform1i("screenTexture", 0);

    shaderTEXT.Bind();
    // glm::mat4 projection_ortho = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    // shaderTEXT.SetUniform4m("projection", projection_ortho);
    shaderTEXT.SetUniform1i("text", 0);

    // Model Model_kafuka("./model/kafuka/kafuka.obj");
    // Model Model_Aru("./model/Aru/Aru.obj");
    // Model Model_kafuka_knife("./model/kafuka/knife.obj");
    // Model Model_kafuka_knife("./model/kafuka/knife.obj");
    // Model Model_raiden("./model/raiden/raiden.obj");
    // Model ourModel_Syabugyo("C:/Users/R/Desktop/model/genshin/Mondstadt/Md_Church/church.obj");
    // Model ourModel_Syabugyo("./model/Dq_Syabugyo/IndoorScene_Dq_Syabugyo.obj");
    // Model ourModel_Syabugyo("./model/Dq_Uyutei/IndoorScene_Dq_Uyutei.obj");
    // texture_dir_output = true;
    Model Model_mistsplitter_reforged("./model/mistsplitter-reforged/mistsplitter-reforged.obj");
    Model Model_DamageHelmet("./model/DamagedHelmet/glTF/DamagedHelmet.gltf");

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    BufferLayout layout_Cubes;
    layout_Cubes.AddFloat(3, 3, 2);
    VertexArray VAO_Cubes;
    VertexBuffer VBO_Cubes(CubesVertices, sizeof(CubesVertices));
    VAO_Cubes.AddBuffer(VBO_Cubes, layout_Cubes);

    BufferLayout layout_Frame;
    layout_Frame.AddFloat(3, 2);
    VertexArray VAO_Frame;
    VertexBuffer VBO_Frame(FrameVertices, sizeof(FrameVertices));
    VAO_Frame.AddBuffer(VBO_Frame, layout_Frame);

    // lights
    // ------------------------------------
    glm::vec3 lightPositions[] = {
        glm::vec3(-10.0f, 10.0f, 10.0f),
        glm::vec3(10.0f, 10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3(10.0f, -10.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)};
    // int nrRows = 7;
    // int nrColumns = 7;
    // float spacing = 2.5;

    // pbr: setup framebuffer
    // ----------------------
    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: load the HDR environment map
    // ---------------------------------
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    // float *data = stbi_loadf("./src/image/dikhololo_night_4k.hdr", &width, &height, &nrComponents, 0);
    float *data = stbi_loadf("./src/image/newport_loft.hdr", &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    VAO_Cubes.Bind();
    shaderHDR.Bind();
    shaderHDR.SetUniform1i("equirectangularMap", 0);
    shaderHDR.SetUniform4m("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        shaderHDR.SetUniform4m("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    shaderIrradiance.Bind();
    shaderIrradiance.SetUniform1i("environmentMap", 0);
    shaderIrradiance.SetUniform4m("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        shaderIrradiance.SetUniform4m("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16, 128, 128, 0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // be sure to set minification filter to mip_linear
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    shaderPrefilter.Bind();
    shaderPrefilter.SetUniform1i("environmentMap", 0);
    shaderPrefilter.SetUniform4m("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // resize framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);
        float roughness = (float)mip / (float)(maxMipLevels - 1);
        shaderPrefilter.SetUniform1f("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            shaderPrefilter.SetUniform4m("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);
    // pre-allocate enough memory for the LUT texture;
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);
    glViewport(0, 0, 512, 512);
    VAO_Frame.Bind();
    shaderBRDF.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // initialize static shader uniforms before rendering
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glm::mat4 projection = glm::perspective(glm::radians(camera.m_Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, zbuffer_near, zbuffer_far);

    while (!glfwWindowShouldClose(window))
    {
        // glCheckError();
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
#if 1 // -----------------------------------imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Lighting Settings");
        imgui_window_focus = ImGui::IsWindowFocused();
        ImGui::PushItemWidth(150);
        ImGui::ColorEdit3(" point", (float *)&lightColor);
        ImGui::Text("Pos: %0.2f,%0.2f,%0.2f", camera.m_cameraPos.x, camera.m_cameraPos.y, camera.m_cameraPos.z);
        ImGui::Text("(%.3f ms)(%.1f fps)", 1000.0f / io.Framerate, io.Framerate);
        // ImGui::SliderFloat("Fresnel", &_fresnel, 0.0, 4.0);
        // ImGui::ColorEdit3("sphere color", (float *)&sphereColor);
        ImGui::ColorEdit3("Text color", (float *)&text_color);
        ImGui::SliderFloat("Text Size", &text_size, 0.005, 0.1);
        ImGui::SliderFloat("Line Spac", &line_spec, 0.0, 10.0);
        // ImGui::Text(text_string.c_str(), sizeof(text_string));
        if (ImGui::InputTextMultiline("Input Text", text_buffer, sizeof(text_buffer), ImVec2(-1, ImGui::GetTextLineHeight() * 4), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            text_string = text_buffer;
            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
            u32text_string = converter.from_bytes(text_string);
            if (!text_changed)
                text_changed = true;
        }
        ImGui::RadioButton("Normal", &SDF_Mode, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Bloom", &SDF_Mode, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Gradient", &SDF_Mode, 2);
        ImGui::SameLine();
        ImGui::RadioButton("Shadow", &SDF_Mode, 3);
        ImGui::End();
#endif

        // glEnable(GL_DEPTH_TEST);
        // glClearColor(background_color.x, background_color.y, background_color.z, 1.0f);

        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 1 // ------------------------------------- normal rendering
      // {
        glm::mat4 view;
        view = camera.GetViewMatrix();
        // float aspect = 1.0f;
        // if (SCR_HEIGHT != 0 || SCR_WIDTH != 0)
        // {
        // aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
        // }
        // glm::mat4 projection = glm::mat4(1.0f);
        // projection = glm::perspective(glm::radians(camera.m_Fov), aspect, zbuffer_near, zbuffer_far);
        // projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -0.01f, 1000.0f);
        // glm::mat4 model = glm::mat4(1.0f);

#if 1 // -------------------------------------------object rendering PBR
      // shaderPBR.Bind();
      // shaderPBR.SetUniform4m("view", view);
      // shaderPBR.SetUniform3f("camPos", camera.m_cameraPos);
      // shaderPBR.SetUniform4m("projection", projection);
      // shaderPBR.SetUniform3f("albedo", sphereColor);
      // glActiveTexture(GL_TEXTURE0);
      // glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
      // glActiveTexture(GL_TEXTURE1);
      // glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
      // glActiveTexture(GL_TEXTURE2);
      // glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        glm::mat4 model = glm::mat4(1.0f);
        {
            // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
            // for (int row = 0; row < nrRows; ++row)
            // {
            //     shaderPBR.SetUniform1f("metallic", (float)row / (float)nrRows);
            //     for (int col = 0; col < nrColumns; ++col)
            //     {
            //         // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
            //         // on direct lighting.
            //         shaderPBR.SetUniform1f("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

            //         model = glm::mat4(1.0f);
            //         model = glm::translate(model, glm::vec3(
            //                                           (col - (nrColumns / 2)) * spacing,
            //                                           (row - (nrRows / 2)) * spacing,
            //                                           -2.0f));
            //         shaderPBR.SetUniform4m("model", model);
            //         shaderPBR.SetUniform3m("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            //         renderSphere();
            //     }
            // }

            // render light source (simply re-render sphere at light positions)
            // this looks a bit off as we use the same shader, but it'll make their positions obvious and
            // keeps the codeprint small.
            // for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
            // {
            //     glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            //     // lightPositions[i] = newPos;
            //     newPos = lightPositions[i];
            //     shaderPBR.SetUniform3f("lightPositions[" + std::to_string(i) + "]", newPos);
            //     shaderPBR.SetUniform3f("lightColors[" + std::to_string(i) + "]", lightColors[i]);

            //     // model = glm::mat4(1.0f);
            //     // model = glm::translate(model, newPos);
            //     // model = glm::scale(model, glm::vec3(0.5f));
            //     // shaderPBR.SetUniform4m("model", model);
            //     // shaderPBR.SetUniform3m("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            //     // renderSphere();
            // }

            // ------------------------- render light sphere
            shaderLightBox.Bind();
            shaderLightBox.SetUniform4m("view", view);
            shaderLightBox.SetUniform4m("projection", projection);
            for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, lightPositions[i]);
                model = glm::scale(model, glm::vec3(0.5f));
                shaderLightBox.SetUniform4m("model", model);
                shaderLightBox.SetUniform3f("lightColor", lightColor * 400.0f);
                lightColors[i] = lightColor * 400.0f;
                renderSphere();
            }

            // ------------------------------- rendering pbr texture
            shaderPBRtexture.Bind();
            shaderPBRtexture.SetUniform4m("view", view);
            shaderPBRtexture.SetUniform3f("camPos", camera.m_cameraPos);
            shaderPBRtexture.SetUniform4m("projection", projection);
            shaderPBRtexture.SetUniform1b("hasEmissive", 0);
            T_albedoMap.Bind();
            T_normalMap.Bind();
            T_metallicMap.Bind();
            // T_roughnessMap.Bind();
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, T_metallicMap.GetTextureID());
            T_aoMap.Bind();
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
            for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
            {
                glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
                // lightPositions[i] = newPos;
                newPos = lightPositions[i];
                shaderPBRtexture.SetUniform3f("lightPositions[" + std::to_string(i) + "]", newPos);
                shaderPBRtexture.SetUniform3f("lightColors[" + std::to_string(i) + "]", lightColors[i]);
            }
            shaderPBRtexture.SetUniform3m("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5f));
            shaderPBRtexture.SetUniform4m("model", model);
            renderSphere();

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
            model = glm::rotate(model, glm::radians((float)90), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
            shaderPBRtexture.SetUniform1b("hasEmissive", 1);
            shaderPBRtexture.SetUniform4m("model", model);
            shaderPBRtexture.SetUniform3m("normalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
            Model_DamageHelmet.Draw(shaderPBRtexture);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.5f, 2.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
            model = glm::rotate(model, glm::radians((float)45), glm::normalize(glm::vec3(-1.0, 0.0, 0.0)));
            shaderPBRtexture.SetUniform1b("hasEmissive", 1);
            shaderPBRtexture.SetUniform4m("model", model);
            shaderPBRtexture.SetUniform3m("normalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
            Model_mistsplitter_reforged.Draw(shaderPBRtexture);

            // VAO_Cubes.Bind();
            // shaderHDR.Bind();
            // shaderHDR.SetUniform4m("view", view);
            // shaderHDR.SetUniform4m("projection", projection);
            // glActiveTexture(GL_TEXTURE0);
            // glBindTexture(GL_TEXTURE_2D, hdrTexture);
            // glDrawArrays(GL_TRIANGLES, 0, 36);

            VAO_Cubes.Bind();
            shaderSKY.Bind();
            shaderSKY.SetUniform4m("projection", projection);
            shaderSKY.SetUniform4m("view", view);
            glActiveTexture(GL_TEXTURE0);
            // shaderSKY.SetUniform1f("_fresnel", _fresnel);
            // glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
            // glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // glViewport(0, 0, 80, 80);
            // VAO_Frame.Bind();
            // Program_buffe.Bind();
            // // shaderBRDF.Bind();
            // glActiveTexture(GL_TEXTURE0);
            // glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
            // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            // glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

            glEnable(GL_BLEND);
            // glDisable(GL_DEPTH_TEST);
            // glDepthMask(GL_FALSE);
            glDepthMask(GL_FALSE);
            shaderTEXT.Bind();
            // shaderTEXT.SetUniform4m("projection", projection_ortho);
            shaderTEXT.SetUniform4m("projection", projection);
            shaderTEXT.SetUniform4m("view", view);
            // RenderTextUniCode(shaderTEXT, u32text_string, 0.0, (FONT_SIZE + 8) * 3, 1.0, text_color);
            RenderTextUniCode(shaderTEXT, u32text_string, 0.0, 0.0, text_size, text_color);
            glDepthMask(GL_TRUE);
            // glEnable(GL_DEPTH_TEST);
            // RenderText(shaderTEXT, text_string.c_str(), 25.0f, 25.0f, text_size, glm::vec3(0.5, 0.8f, 0.2f));
            // glDepthMask(GL_TRUE);
            // RenderText(shaderTEXT, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
            glDisable(GL_BLEND);
        }
#endif

#endif

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    camera.saveCameraPosition(window);

    glfwTerminate();
    return 0;
}

int opengl_init()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "mashiro", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    camera.loadCameraPosition(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetScrollCallback(window, scroll_callback);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // glEnable(GL_MULTISAMPLE); // Enabled by default on some drivers, but not all so always enable to make sure
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    // glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_FRAMEBUFFER_SRGB);

    // glEnable(GL_CULL_FACE);

    return 0;
}

// void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] int width, [[maybe_unused]] int height)
// {
// make sure the viewport matches the new window dimensions; note that width and height
// will be significantly larger than specified on retina displays
// glViewport(0, 0, width, height);
// SCR_WIDTH = width;
// SCR_HEIGHT = height;
// int x = (width - SCR_WIDTH) / 4;
// int y = (height - SCR_HEIGHT) / 4;
// cout << width << " -/2 " << SCR_WIDTH << "=" << x << " " << height << " -/2 " << SCR_HEIGHT << "=" << y << endl;
// glViewport(x, y, SCR_WIDTH, SCR_HEIGHT);
// }

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (!imgui_window_focus)
    {
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera.ProcessKeyBoard(MOVE_RIGHT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera.ProcessKeyBoard(MOVE_LEFT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            camera.ProcessKeyBoard(MOVE_FORWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            camera.ProcessKeyBoard(MOVE_BACKWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            camera.ProcessKeyBoard(MOVE_UP, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            camera.ProcessKeyBoard(MOVE_DOWN, deltaTime);
        }
    }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)(window);
    (void)(scancode);
    (void)(mods);
    if (key == GLFW_KEY_LEFT_CONTROL)
    {
        controlKey = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    (void)window;
    static float lastX = xpos, lastY = ypos;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }
    if (!rightClick && !imgui_window_focus)
    {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    static float speed_move = 2.0f;
    (void)window;
    (void)xoffset;
    if (!rightClick && controlKey)
    {
        camera.ProcessMouseScroll(yoffset);
    }
    else if (yoffset > 0)
    {
        speed_move += (speed_move < 1.0f) ? 0.1f : 0.5f;
        // speed_move += 0.1;
        camera.SetSpeed(speed_move);
    }
    else if (yoffset < 0)
    {
        speed_move -= (speed_move <= 1.0f) ? 0.1f : 0.5f;
        // speed_move -= 0.1;
        speed_move = std::max(speed_move, 0.1f);
        camera.SetSpeed(speed_move);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && !imgui_window_focus)
    {
        static bool flag = true;
        if (flag)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            rightClick = false;
            firstMouse = true;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            rightClick = true;
        }
        flag = !flag;
    }
    // else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    // {
    // }
}

void SceneLightConfig(Shader &shader, glm::mat4 view, glm::mat4 projection)
{
    shader.Bind();
    shader.SetUniform4m("view", view);
    shader.SetUniform4m("projection", projection);
    // shader.SetUniform1f("scales", 1.0f);
    // direction light
    // shader.SetUniform1i("depth_test", (int)depth_test);
    // shader.SetUniform1f("zbuffer_near", zbuffer_near);
    // shader.SetUniform1f("zbuffer_far", zbuffer_far);
    // shader.SetUniform1f("refractive_rate", refractive_rate);

    // shader.SetUniform3f("dirLight.direction", sunlight_pos);
    // shader.SetUniform3f("dirLightDirecation", sunlight_pos);
    // shader.SetUniform3f("dirLight.ambient", sunlight_color * light_am_di_sp_directional.x);
    // shader.SetUniform3f("dirLight.diffuse", sunlight_color * light_am_di_sp_directional.y);
    // shader.SetUniform3f("dirLight.specular", sunlight_color * light_am_di_sp_directional.z);
    // point light
    shader.SetUniform3f("pointLightPos", lightPos);
    shader.SetUniform3f("viewPos", camera.m_cameraPos);
    shader.SetUniform3f("pointLight.position", lightPos);
    shader.SetUniform3f("pointLight.ambient", lightColor_point * light_am_di_sp_point.x);
    shader.SetUniform3f("pointLight.diffuse", lightColor_point * light_am_di_sp_point.y);
    shader.SetUniform3f("pointLight.specular", lightColor_point * light_am_di_sp_point.z);
    shader.SetUniform1f("pointLight.constant", 1.0f);
    shader.SetUniform1f("pointLight.linear", light_distance[light_distance_select_point][0]);
    shader.SetUniform1f("pointLight.quadratic", light_distance[light_distance_select_point][1]);
    // spotLight
    // shader.SetUniform3f("spotLight.position", lightSpotPos);
    // shader.SetUniform3f("spotLightPos", lightSpotPos);
    // shader.SetUniform3f("spotLight.direction", lightDirection);
    // shader.SetUniform3f("spotLightDirecation", lightDirection);
    // shader.SetUniform3f("spotLight.ambient", lightColor_spot * light_am_di_sp_spot.x);
    // shader.SetUniform3f("spotLight.diffuse", lightColor_spot * light_am_di_sp_spot.y);
    // shader.SetUniform3f("spotLight.specular", lightColor_spot * light_am_di_sp_spot.z);
    // shader.SetUniform1f("spotLight.constant", 1.0f);
    // shader.SetUniform1f("spotLight.linear", light_distance[light_distance_select_spot][0]);
    // shader.SetUniform1f("spotLight.quadratic", light_distance[light_distance_select_spot][1]);
    // shader.SetUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    // shader.SetUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
    // shader.SetUniform1i("material.shininess", material_shininess);
}

// FrameBuffer framebuffer_intermediate(SCR_WIDTH, SCR_HEIGHT, GL_TEXTURE_2D);
// FrameBuffer framebuffer_intermediate1(SCR_WIDTH, SCR_HEIGHT, GL_TEXTURE_2D);

// unsigned int hdrFBO;
// glGenFramebuffers(1, &hdrFBO);
// glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
// unsigned int colorBuffers[2];
// glGenTextures(2, colorBuffers);
// for (unsigned int i = 0; i < 2; i++)
// {
//     glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBuffers[i]);
//     glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, SCR_WIDTH * 4, SCR_HEIGHT * 4, GL_TRUE);
//     glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, colorBuffers[i], 0);
// }
// unsigned int rboDepth;
// glGenRenderbuffers(1, &rboDepth);
// glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
// glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH * 4, SCR_HEIGHT * 4);
// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
// unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
// glDrawBuffers(2, attachments);
// // finally check if framebuffer is complete
// if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//     std::cout << "Framebuffer not complete!" << std::endl;
// glBindFramebuffer(GL_FRAMEBUFFER, 0);

// unsigned int pingpongFBO[2];
// unsigned int pingpongColorbuffers[2];
// glGenFramebuffers(2, pingpongFBO);
// glGenTextures(2, pingpongColorbuffers);
// for (unsigned int i = 0; i < 2; i++)
// {
//     glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
//     glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
//     // also check if framebuffers are complete (no need for depth buffer)
//     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//         std::cout << "Framebuffer not complete!" << std::endl;
// }

// glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
// glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// /*rendering-------------------------------------------------------------------------------------*/
// glBindFramebuffer(GL_READ_FRAMEBUFFER, hdrFBO);
// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_intermediate.GetFrameBufferID());
// glReadBuffer(GL_COLOR_ATTACHMENT0);
// glDrawBuffer(GL_COLOR_ATTACHMENT0);
// glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

// glBindFramebuffer(GL_READ_FRAMEBUFFER, hdrFBO);
// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_intermediate1.GetFrameBufferID());
// glReadBuffer(GL_COLOR_ATTACHMENT1);
// glDrawBuffer(GL_COLOR_ATTACHMENT0);
// glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<unsigned int>(indices.size());

        // calculate tangents
        // for (unsigned int i = 0; i < indices.size() - 2; i += 2)
        // {
        //     unsigned int index0 = indices[i];
        //     unsigned int index1 = indices[i + 1];
        //     unsigned int index2 = indices[i + 2];

        //     glm::vec3 v0 = positions[index0];
        //     glm::vec3 v1 = positions[index1];
        //     glm::vec3 v2 = positions[index2];

        //     glm::vec2 uv0 = uv[index0];
        //     glm::vec2 uv1 = uv[index1];
        //     glm::vec2 uv2 = uv[index2];

        //     glm::vec3 deltaPos1 = v1 - v0;
        //     glm::vec3 deltaPos2 = v2 - v0;

        //     glm::vec2 deltaUV1 = uv1 - uv0;
        //     glm::vec2 deltaUV2 = uv2 - uv0;

        //     float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        //     glm::vec3 tangent;
        //     tangent.x = r * (deltaUV2.y * deltaPos1.x - deltaUV1.y * deltaPos2.x);
        //     tangent.y = r * (deltaUV2.y * deltaPos1.y - deltaUV1.y * deltaPos2.y);
        //     tangent.z = r * (deltaUV2.y * deltaPos1.z - deltaUV1.y * deltaPos2.z);

        //     tangents.push_back(tangent);
        //     if(i == indices.size() - 4)
        //     {
        //         tangents.push_back(tangent);
        //         tangents.push_back(tangent);
        //         tangents.push_back(tangent);
        //         tangents.push_back(tangent);
        //     }
        // }
        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 3 + 2) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

// render line of text
// -------------------
void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state
    shader.Bind();
    shader.SetUniform3f("texColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];
        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos, ypos, 0.0f, 1.0f},
            {xpos + w, ypos, 1.0f, 1.0f},
            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos + w, ypos, 1.0f, 1.0f},
            {xpos + w, ypos + h, 1.0f, 0.0f}};

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}