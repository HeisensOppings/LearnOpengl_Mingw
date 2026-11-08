#ifndef __IMAGELOADER_H__
#define __IMAGELOADER_H__

// Image decoding through Qt (QImage), replacing stb_image.h.

#include <string>
#include <vector>

struct ImageData
{
    int width = 0;
    int height = 0;
    int channels = 0;                 // 1 = R, 3 = RGB, 4 = RGBA (like stbi with req_comp = 0)
    std::vector<unsigned char> pixels; // tightly packed rows, top row first

    bool valid() const { return width > 0 && height > 0 && !pixels.empty(); }
    const unsigned char *data() const { return pixels.data(); }
};

// Loads an image file with QImage. Thread-safe (QImage may be used outside the GUI thread).
// flipVertically mirrors the image like stbi_set_flip_vertically_on_load(true).
bool LoadImageFile(const std::string &path, ImageData &out, bool flipVertically = false);

#endif
