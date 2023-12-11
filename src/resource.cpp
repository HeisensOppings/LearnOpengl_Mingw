#include "resource.h"

std::unordered_map<std::string, Texture> ResourceManager::Textures;
std::unordered_map<std::string, Shader> ResourceManager::Shaders;
std::unordered_map<std::string, VertexArray> ResourceManager::VAOs;

void ResourceManager::ShaderInit(string name)
{
    Shader::ShaderInit(name);
}

void ResourceManager::Clear()
{
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.GetID());
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.GetTextureID());
}

Shader &ResourceManager::SetShader(string name, int vertex, int fragment, int geometry)
{
    auto it = Shaders.find(name);
    if (it == Shaders.end())
        return (Shaders.emplace(std::piecewise_construct,
                                std::forward_as_tuple(name), std::forward_as_tuple(vertex, fragment, geometry)))
            .first->second;
    else
        return (*it).second;
}

Shader &ResourceManager::GetShader(std::string name)
{
    try
    {
        return Shaders.at(name);
    }
    catch (const std::out_of_range &e)
    {
        cout << e.what() << ": " << name << endl;
        exit(1);
    }
}

void ResourceManager::LoadTexture(const string name, const string file)
{
    Textures.emplace(name, file);
}

Texture &ResourceManager::GetTexture(std::string name)
{
    try
    {
        return Textures.at(name);
    }
    catch (const std::out_of_range &e)
    {
        cout << e.what() << ": " << name << endl;
        exit(1);
    }
}

void ResourceManager::SetVAO(string name, vector<unsigned int> bufferlayout, const void *vbo_data, unsigned int vbo_size, const int vbo_usage,
                             const void *ibo_data, unsigned int ibo_count, const int ibo_usage)
{
    VAOs.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                 std::forward_as_tuple(bufferlayout, vbo_data, vbo_size, vbo_usage, ibo_data, ibo_count, ibo_usage));
}

VertexArray &ResourceManager::GetVAO(const string name)
{
    try
    {
        return VAOs.at(name);
    }
    catch (const std::out_of_range &e)
    {
        cout << e.what() << ": " << name << endl;
        exit(1);
    }
}

vector<string> font_paths = {
    "./src/fonts/base-split.woff",
    "./src/fonts/HarmonyOS_Sans_SC_Medium.ttf",
    "./src/fonts/NotoSansArabic-Medium.ttf",
    "./src/fonts/NotoSansCanadianAboriginal-Medium.ttf",
    "./src/fonts/NotoSansCuneiform-Regular.ttf",
    "./src/fonts/NotoSansSymbols2-Regular.ttf",
    "./src/fonts/NotoSans-ExtraBold.ttf",
};

map<char32_t, Character> TextTexture::Characters;
GLuint TextTexture::mtextureID = 0;
GLfloat TextTexture::mtextureWidth = 1000.0f;
GLfloat TextTexture::mtextureHeight = 1000.0f;

GLuint TextTexture::ChToTexture(u32string u32str)
{
    bool texture_needs_update = false;
    for (const auto &ch : u32str)
    {
        if (Characters.find(ch) == Characters.end())
        {
            texture_needs_update = true;
            break;
        }
    }
    if (!texture_needs_update)
        return -1;

    static bool first = true;
    if (first)
    {
        glGenTextures(1, &mtextureID);
        glBindTexture(GL_TEXTURE_2D, mtextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mtextureWidth, mtextureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        first = false;
    }

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

    // double rotationAngle = 30.0;
    // FT_Matrix matrix;
    // FT_Vector pen;
    // double radians = rotationAngle * 3.14159 / 180.0;
    // matrix.xx = (FT_Fixed)(cos(radians) * 0x10000);
    // matrix.xy = (FT_Fixed)(-sin(radians) * 0x10000);
    // matrix.yx = (FT_Fixed)(sin(radians) * 0x10000);
    // matrix.yy = (FT_Fixed)(cos(radians) * 0x10000);
    // pen.x = 0;
    // pen.y = 0;

    glBindTexture(GL_TEXTURE_2D, mtextureID);

    for (const auto &ch : u32str)
    {
        if (Characters.find(ch) != Characters.end())
            continue;

        static float texture_sub_x = 0.0f, texture_sub_y = 0.0f;
        static unsigned int row_height = 0;

        for (const auto &face : faces)
        {
            FT_UInt glyphIndex = FT_Get_Char_Index(face, ch);
            if (glyphIndex == 0)
                continue;
            else
            {
                // FT_Set_Transform(face, &matrix, &pen);
                FT_Load_Char(face, ch, FT_LOAD_RENDER);
                FT_GlyphSlot slot = face->glyph;
                // if (hasSDF)
                // {
                FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);
                // }

                Character character;
                if (ch == ' ')
                {
                    character.Advance = static_cast<unsigned int>((face->glyph->advance.x) >> 6);
                    Characters.insert(std::pair<FT_ULong, Character>(ch, character));
                }
                else
                {
                    if (texture_sub_x + slot->bitmap.width + 1 >= mtextureWidth)
                    {
                        texture_sub_y += row_height;
                        texture_sub_x = 0;
                        row_height = 0;
                        if (texture_sub_y >= (mtextureHeight - FONT_SIZE))
                            cout << "text texture no enough" << endl;
                    }

                    character = {
                        glm::ivec2(slot->bitmap.width, slot->bitmap.rows),
                        glm::ivec2(slot->bitmap_left, slot->bitmap_top),
                        static_cast<unsigned int>((slot->advance.x) >> 6),
                        glm::vec4(texture_sub_x / 1000.0f, texture_sub_y / 1000.0f, slot->bitmap.width / 1000.0f, slot->bitmap.rows / 1000.0f)}; // 26.6 or // 2^6=64 ((face)->glyph->advance.x)/64)
                    Characters.insert(std::pair<FT_ULong, Character>(ch, character));

                    glTexSubImage2D(GL_TEXTURE_2D, 0, texture_sub_x, texture_sub_y, slot->bitmap.width, slot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, slot->bitmap.buffer);
                    texture_sub_x += slot->bitmap.width + 1;

                    row_height = std::max(row_height, slot->bitmap.rows);
                }
#if 0
                        if (ch == 'm')
                        {
                            for (unsigned int r = 0; r < face->glyph->bitmap.rows; ++r)
                            {
                                cout << endl;
                                for (unsigned int x = 0; x < face->glyph->bitmap.width; ++x)
                                {
                                    unsigned int index = r * face->glyph->bitmap.width + x;
                                    unsigned int value = face->glyph->bitmap.buffer[index];
                                    cout << setw(4) << (value) << " ";
                                }
                            }
                            cout << endl;
                            cout << "advance.x:" << face->glyph->advance.x << " advance.y:" << face->glyph->advance.y << endl;
                            cout << "bitmap.width:" << face->glyph->bitmap.width << " bitmap.rows:" << face->glyph->bitmap.rows << endl;
                            cout << "bitmap_left:" << face->glyph->bitmap_left << " bitmap_top:" << face->glyph->bitmap_top << endl;
                            cout << (face->size->metrics.height >> 6) << " advance.x>>6:" << ((face->glyph->advance.x) >> 6) << endl;
                        }
#endif
                break;
            }
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    for (auto &face : faces)
    {
        FT_Done_Face(face);
    }
    FT_Done_FreeType(ft);

    return 0;
}
